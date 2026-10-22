# C Source Code Reconstruction (GamBit)

This folder contains reconstructions of the C source code based on the QNX 1.14 raw floppy diskette images saved in `../raw-floppies`.

## Source Code Reconstructions

- `scott_johnson` -- Work by Scott Johnson
- `bill_alewine` -- Work by Bill Alewine
- `alex_rad` -- Work by Alex Rad

Thank you!

## Front End and Back End

The C version of Scepter on QNX was constrained by the 64KB code segment size limit of the Intel 80286 processor (IBM PC XT). Scepter worked around the limit by running two QNX processes with a shared data segment, raising the code size limit to 128KB. 

The two processes were called the 'front end' and the 'back end'.  The front end contained the top-level `select()` dispatch loop and ran most of the low-level I/O code (mwrite).  The back end contained the game logic.

Upon start, the front end process launched the back end process, which immediately executed a HALT instruction and remained memory resident.  The front end then executed inter-segment CALLs to run the code on the back end. The whole game ran on a single thread.  This was possible because there was no memory protection on the Intel 80286.

## Front End Source Code Is Missing

The raw floppy diskette images do not contain the front end source code, unfortunately.

The back end contains the most interesting code historically, including much of the game mechanics.

It should be possible to interpolate much of the missing code using the original code from the Pascal version.  The original C version was mostly a manual transpilation of the Pascal version, adapted for QNX. (Later changes to the C version were mostly focused on the back end, where the game logic was located.)
