#!/usr/bin/env python3
"""Extract files from the 1983-84 GamBit floppy images in reference/floppies.

Most of the disks use the QNX 1.x filesystem. MBBS.img is a PC-DOS 1.1 disk
(FAT12), so it gets a separate, simpler reader.

Usage:
    python3 scripts/extract-floppies.py [IMG ...] [-o reference/recovered]

With no images given, every *.img in reference/floppies is extracted
into reference/recovered.
Python 3.11+ stdlib only.

QNX 1.x layout, as reverse-engineered from these images
-------------------------------------------------------
Blocks are 512 bytes and numbered from 1 (block n is at offset (n-1)*512).
Block 2 is the free-space bitmap and block 3 holds the root directory.

A file is a chain of extents. Each extent is a run of contiguous blocks
whose first 16 bytes are a header:

    u32 prev    block number of the previous extent (0 = first)
    u32 next    block number of the next extent (0 = last)
    u32 used    bytes of file data held in this extent
    u32 blocks  blocks in this extent, minus one

The file data follows the header and runs straight across block
boundaries inside the extent.

A directory is a file whose data is a 4-byte header followed by 48-byte
entries:

    +0x00 u8    status (0x00 = deleted; 0x40 and 0x50 both seen on live files)
    +0x01 u32   first extent block
    +0x05 u32   last extent block
    +0x09 u32   total blocks, minus one
    +0x19 u8    attributes (0x20 = directory, 0x08 = executable)
    +0x1a 5     date/time stamp (format not yet decoded)
    +0x1f 16    name, NUL padded

Text files use 0x1E (ASCII record separator) as the line terminator. The
extractor writes text files with normal newlines and keeps binaries as is.

Deleted entries often still point at intact data. Those are recovered
into a _deleted/ folder, but their blocks may have been reused, so treat
that output as suspect.

Directories that are no longer reachable from the root are found by
scanning for blocks that look like the start of a directory, and recovered
into _orphans/blk<N>/.

SYSDEVPARTA.img and SYSDEVPARTB.img are the two halves of one 1280-block
volume: the root lives in PARTA and points at blocks up to 1269. Any
*PARTA.img / *PARTB.img pair is joined and extracted as one volume.
"""

import argparse
import re
import struct
import sys
from pathlib import Path

BLOCK = 512
ENTRY = 48
DIR_HEADER = 4

ST_DELETED = 0x00
ATTR_DIR = 0x20
ATTR_EXEC = 0x08

REPO = Path(__file__).resolve().parent.parent


class Disk:
    def __init__(self, data):
        self.data = data
        self.nblocks = len(self.data) // BLOCK

    def block(self, n):
        if not 1 <= n <= self.nblocks:
            raise ValueError(f"block {n} out of range")
        return self.data[(n - 1) * BLOCK:n * BLOCK]

    def extent_header(self, n):
        return struct.unpack_from("<IIII", self.block(n))

    def read_file(self, first):
        """Follow the extent chain from `first`. Returns (bytes, extents, problems)."""
        out = bytearray()
        seen = []
        problems = []
        n = first
        prev_expected = 0
        while n:
            if n in seen:
                problems.append(f"extent loop at block {n}")
                break
            try:
                prev, nxt, used, blocks = self.extent_header(n)
            except ValueError as e:
                problems.append(str(e))
                break
            if prev != prev_expected:
                problems.append(f"block {n}: prev={prev}, expected {prev_expected}")
            capacity = (blocks + 1) * BLOCK - 16
            if used > capacity or n + blocks > self.nblocks:
                problems.append(f"block {n}: bad extent (used={used}, blocks={blocks + 1})")
                break
            start = (n - 1) * BLOCK + 16
            out += self.data[start:start + used]
            seen.append(n)
            prev_expected = n
            n = nxt
        return bytes(out), seen, problems

    def read_dir(self, first):
        raw, extents, problems = self.read_file(first)
        entries = []
        for off in range(DIR_HEADER, len(raw) - ENTRY + 1, ENTRY):
            e = raw[off:off + ENTRY]
            name = e[0x1f:0x2f].split(b"\0")[0]
            if not name:
                continue
            status = e[0]
            efirst, elast, eblocks = struct.unpack_from("<III", e, 1)
            if not valid_name(name) or not 1 <= efirst <= self.nblocks:
                continue
            entries.append({
                "name": name.decode("latin-1"),
                "status": status,
                "first": efirst,
                "last": elast,
                "blocks": eblocks + 1,
                "attr": e[0x19],
                "stamp": e[0x1a:0x1f].hex(),
            })
        return entries, extents, problems

    def looks_like_dir_start(self, n):
        """True if block n starts a directory: a first extent whose data is
        a whole number of entries, every one of them plausible."""
        prev, nxt, used, blocks = self.extent_header(n)
        if prev != 0 or used < DIR_HEADER + ENTRY:
            return False
        # A one-extent directory holds whole entries; a longer one may split
        # an entry across the extent boundary.
        if nxt == 0 and (used - DIR_HEADER) % ENTRY:
            return False
        if (blocks + 1) * BLOCK - 16 < used or n + blocks > self.nblocks:
            return False
        b = self.block(n)
        shown = min((used - DIR_HEADER) // ENTRY, (BLOCK - 16 - DIR_HEADER) // ENTRY)
        for i in range(shown):
            e = b[16 + DIR_HEADER + i * ENTRY:16 + DIR_HEADER + (i + 1) * ENTRY]
            name = e[0x1f:0x2f].split(b"\0")[0]
            first = struct.unpack_from("<I", e, 1)[0]
            if not valid_name(name) or not 1 <= first <= self.nblocks:
                return False
        return True


def valid_name(name):
    return 0 < len(name) <= 16 and all(32 < c < 127 for c in name)


def is_text(raw):
    if not raw:
        return True
    ok = sum(1 for c in raw if 32 <= c < 127 or c in (9, 10, 12, 13, 0x1e))
    return ok / len(raw) > 0.95


def to_text(raw):
    return raw.replace(b"\x1e", b"\n").decode("latin-1")


def safe_name(name):
    return "".join(c if c.isalnum() or c in "._-" else "_" for c in name) or "_"


def write_file(dest, raw):
    dest.parent.mkdir(parents=True, exist_ok=True)
    # Deleted copies can share a name; keep them all.
    n = 2
    stem = dest
    while dest.exists():
        dest = stem.with_name(f"{stem.name}~{n}")
        n += 1
    if is_text(raw):
        dest.write_text(to_text(raw), encoding="latin-1", newline="\n")
        return dest, "text"
    dest.write_bytes(raw)
    return dest, "binary"


def extract_qnx(disk, outdir):
    manifest = []
    visited_dirs = set()
    used_blocks = set()

    def mark_used(extents):
        for n in extents:
            used_blocks.update(range(n, n + disk.extent_header(n)[3] + 1))

    def walk(first, rel, deleted):
        if first in visited_dirs:
            return
        visited_dirs.add(first)
        entries, extents, problems = disk.read_dir(first)
        mark_used(extents)
        for p in problems:
            manifest.append((str(rel) + "/", "dir", "", "", f"PROBLEM: {p}"))
        for e in entries:
            if rel == Path(".") and e["name"] == "bitmap":
                continue
            gone = deleted or e["status"] == ST_DELETED
            # The first deleted entry on a path moves its whole subtree under _deleted/.
            child = rel / safe_name(e["name"])
            if gone and not deleted:
                child = Path("_deleted") / child
            if e["attr"] & ATTR_DIR:
                walk(e["first"], child, gone)
                continue
            raw, ext, problems = disk.read_file(e["first"])
            mark_used(ext)
            target, kind = write_file(outdir / child, raw)
            note = "; ".join(problems)
            if gone:
                note = ("DELETED entry; " + note).strip("; ")
            manifest.append((
                str(target.relative_to(outdir)), kind, len(raw), len(ext),
                f"attr={e['attr']:02x} stamp={e['stamp']} {note}".strip(),
            ))

    if disk.looks_like_dir_start(3):
        walk(3, Path("."), False)
    else:
        manifest.append(("(root)", "dir", "", "", "PROBLEM: no root directory at block 3"))

    # Directories nobody points at any more.
    for n in range(1, disk.nblocks + 1):
        if n in visited_dirs or n in used_blocks:
            continue
        if disk.looks_like_dir_start(n):
            walk(n, Path("_orphans") / f"blk{n}", True)

    manifest += recover_free_space(disk, outdir, used_blocks | {1, 2})
    return manifest


def plausible_header(disk, n):
    prev, nxt, used, blocks = disk.extent_header(n)
    return (prev <= disk.nblocks and nxt <= disk.nblocks
            and 0 < used <= (blocks + 1) * BLOCK - 16
            and n + blocks <= disk.nblocks)


def recover_free_space(disk, outdir, used_blocks):
    """Pull text out of blocks no directory entry claims.

    First, any free block that starts a clean extent chain is read as a
    whole file and named after its leading /* name - ... */ comment.
    Whatever text is left is written as raw fragments, one per run of
    consecutive free blocks. Fragments can mix old and new versions of a
    file; they are material for hand stitching, not finished files."""
    manifest = []
    used = set(used_blocks)
    files_dir = outdir / "_unallocated" / "files"
    frag_dir = outdir / "_unallocated" / "fragments"

    for n in range(1, disk.nblocks + 1):
        if n in used or not plausible_header(disk, n) or disk.extent_header(n)[0] != 0:
            continue
        raw, ext, problems = disk.read_file(n)
        spans = {b for x in ext for b in range(x, x + disk.extent_header(x)[3] + 1)}
        if problems or not raw or spans & used or not is_text(raw):
            continue
        used |= spans
        m = re.match(r"\s*/\*\s*([A-Za-z_0-9.]+)", to_text(raw))
        name = safe_name(m.group(1)) if m else f"blk{n}"
        target, kind = write_file(files_dir / name, raw)
        manifest.append((str(target.relative_to(outdir)), kind, len(raw), len(ext),
                         f"FREE SPACE: unlisted extent chain at block {n}"))

    run = []
    for n in range(1, disk.nblocks + 2):
        if n <= disk.nblocks and n not in used:
            run.append(n)
            continue
        if run:
            chunks = []
            for b in run:
                blk = disk.block(b)
                chunks.append(blk[16:] if plausible_header(disk, b) else blk)
            text = bytes(c for c in b"".join(chunks)
                         if 32 <= c < 127 or c in (9, 10, 0x1e))
            if len(text) >= 64:
                name = f"blk{run[0]}-{run[-1]}.txt"
                target, kind = write_file(frag_dir / name, text)
                manifest.append((str(target.relative_to(outdir)), kind, len(text), "",
                                 "FREE SPACE: raw text fragment"))
            run = []
    return manifest


def extract_fat12(disk_path, outdir):
    """PC-DOS 1.1, 320 KB double-sided: 1 boot sector, 2 x 1-sector FATs,
    7-sector root (112 entries), 2 sectors per cluster."""
    d = disk_path.read_bytes()
    fat = d[0x200:0x400]
    root = d[0x600:0x600 + 7 * 512]
    data_start = 10 * 512
    cluster = 1024

    def next_cluster(c):
        off = c * 3 // 2
        v = fat[off] | fat[off + 1] << 8
        return v >> 4 if c & 1 else v & 0xFFF

    manifest = []
    for i in range(0, len(root), 32):
        e = root[i:i + 32]
        if e[0] in (0x00, 0xE5):
            continue
        name = e[0:8].decode("latin-1").rstrip()
        ext = e[8:11].decode("latin-1").rstrip()
        fname = safe_name(f"{name}.{ext}" if ext else name)
        start, size = struct.unpack_from("<HI", e, 26)
        raw = bytearray()
        c = start
        seen = set()
        while 2 <= c < 0xFF8 and c not in seen and len(raw) < size:
            seen.add(c)
            off = data_start + (c - 2) * cluster
            raw += d[off:off + cluster]
            c = next_cluster(c)
        raw = bytes(raw[:size])
        _, kind = write_file(outdir / fname, raw)
        note = "" if len(raw) == size else f"PROBLEM: short read {len(raw)}/{size}"
        manifest.append((fname, kind, len(raw), len(seen), note))
    return manifest


def is_fat(path):
    head = path.read_bytes()[:3]
    return head[0] in (0xEB, 0xE9)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("images", nargs="*", type=Path)
    ap.add_argument("-o", "--out", type=Path, default=REPO / "reference" / "recovered")
    args = ap.parse_args()

    images = args.images or sorted((REPO / "reference" / "floppies").glob("*.img"))
    if not images:
        sys.exit("no images found")

    # Join PARTA/PARTB halves into one volume.
    volumes = []
    for img in images:
        if img.stem.endswith("PARTB") and img.with_name(img.stem[:-1] + "A.img") in images:
            continue
        partb = img.with_name(img.stem[:-1] + "B.img")
        if img.stem.endswith("PARTA") and partb in images:
            volumes.append((img.stem[:-5], [img, partb]))
        else:
            volumes.append((img.stem, [img]))

    for name, parts in volumes:
        outdir = args.out / name
        if outdir.exists() and any(outdir.iterdir()):
            sys.exit(f"{outdir} is not empty; remove it first")
        if len(parts) == 1 and is_fat(parts[0]):
            manifest = extract_fat12(parts[0], outdir)
            fs = "PC-DOS FAT12"
        else:
            manifest = extract_qnx(Disk(b"".join(p.read_bytes() for p in parts)), outdir)
            fs = "QNX 1.x"
        outdir.mkdir(parents=True, exist_ok=True)
        lines = ["path\tkind\tbytes\textents\tnotes"]
        lines += ["\t".join(str(x) for x in row) for row in manifest]
        (outdir / "_MANIFEST.tsv").write_text("\n".join(lines) + "\n")
        files = [r for r in manifest if r[1] in ("text", "binary")]
        total = sum(r[2] for r in files)
        label = "+".join(p.name for p in parts)
        print(f"{label:34} {fs:13} {len(files):3} files  {total:7} bytes  -> {outdir}")


if __name__ == "__main__":
    main()
