Scepter of Goth: files recovered from the 1983-84 GamBit QNX floppies
=====================================================================

Extracted October 2026 by Bill Alewine from the floppy images in
sej69/ScepterOfGothDocs (source/raw-floppies), using extract-floppies.py
(included; Python 3.11+, standard library only). The QNX 1.x disk format
is documented at the top of the script, as worked out from these images.

To re-run it, put the .img files in reference/floppies/ beside a
scripts/ folder holding the script, then:

    python3 scripts/extract-floppies.py -o recovered

What came off each disk
-----------------------

  SCEPTER/          SCEPTER.img: 69 files. Scepter's back end (combat,
                    death and obituaries, saving, display, room and player
                    files, parser, the RFM record-file layer), the linker
                    map and link order, data/elist.base (258 monster
                    records), and deleted room data.
  SYSDEVCHANGES/    SYSDEVCHANGES.img: 69 files. Combat, the 1984
                    multi-user spaceship game, complete, plus 29 GamBit
                    system files rescued from free space.
  SYSDEV/           SYSDEVPARTA.img + SYSDEVPARTB.img: 275 files. These
                    are the two halves of one 1280-block volume (the root
                    is in PARTA and points into PARTB). A QNX/GamBit
                    system volume: C library, commands, help text, and
                    the GamBit programs (accounting, menu, login shell,
                    talk, monitor).
  DEVELOPMENTMAST/  DEVELOPMENTMAST.img: 118 files. QNX and GamBit headers
                    (def/, including multidef.h), C library, commands.
  EXCHTRANSFER/     EXCHTRANSFER.img: 18 files. GamBit help screens.

MBBS.img (a PC-DOS 1.1 disk with the Minnesota BBS program and IBM's DOS
system files) is left out of this copy.

Every folder has a _MANIFEST.tsv listing each file, its size, how many
pieces it was stored in, and any problems found. Three special
subfolders hold less certain material:

  _deleted/                  files whose directory entry was marked
                             deleted; their blocks may have been reused.
  _unallocated/files/        complete files found in free space that no
                             directory lists any more, named after the
                             comment at the top of the file.
  _unallocated/fragments/    leftover text from free space, one file per
                             run of free blocks, mixing old and new
                             versions; raw material for piecing together.

How complete it is
------------------

Measured against the strings dumps that came with each image: the share
of text lines (25+ characters) in each dump that appear in a recovered
file.

  Disk              In real files   Counting free-space fragments too
  SCEPTER           94%             100%
  DEVELOPMENTMAST   100%            100%
  SYSDEVPARTA       100%            100%
  SYSDEVPARTB       94%             99%
  SYSDEVCHANGES     82%             100%

The leftover lines are mostly older versions of the same code in free
blocks. All 82 recovered .c files that start with a header comment match
their filename, except renamed copies and scratch files.

Notes
-----

- Text files used byte 0x1E as the line break; the extractor converts it
  to a newline and leaves binaries as they are.
- SCEPTER/back/file/test.c is really room.h: the room record layout.
- The front end (/sys/scept_child) and the headers scepter.h, user.h,
  monster.h, object.h and globals.h are not on any of the disks.
- Room descriptions survive only in SCEPTER/_deleted/data/edescrp_bak2
  and the fragments blk521-556, blk558-629 and blk635-640.
