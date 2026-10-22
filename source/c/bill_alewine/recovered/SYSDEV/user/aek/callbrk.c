/*		callbrk.c - call, transform, and break routines.
		83/11/06.	A. E. Klietz.
*/


#include <stdio.h>
#include <dev.h>
#include "aekdef.h"

#define BEQUEATH_TTY 0x04

extern char  My_priority;


call(cmd, arg1, arg2)
/* Procedure call - call another program and return. */

char	*cmd, *arg1, *arg2;
{
	create(0, BEQUEATH_TTY, My_priority, 0, 0, cmd, arg1, arg2, 0);
}


transform(cmd, arg1, arg2)
/* Procedure transform - change into another program and do not return. */

char	*cmd, *arg1, *arg2;
{
	create(1, BEQUEATH_TTY, My_priority, 0, 0, cmd, arg1, arg2, 0);
}

break_handler()
/* Procedure break_handler - process break key. */

{
	extern int Exc_bits, Exc_pending;
	struct	dev_entry	buffer;	
	char	ch, devname[8];


		get_attr(stdout, &buffer);

/*	Add leading dollar sign to make legal device name. */
	
		devname[0] = '$';
		strcpy(&devname[1],buffer.dev_name);
		fclose(stdout);
		if ((stdout = fopen(devname, "wv")) == 0)
			error("break:fatal - cant reopen output.\n");
        fclose(stdin);
        if ((stdin = fopen(devname, "rv")) == 0)
           error("break:fatal - cant reopen input.\n");
	reset();
}
