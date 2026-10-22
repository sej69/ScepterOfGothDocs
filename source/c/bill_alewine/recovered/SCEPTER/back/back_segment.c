/*		back_segment - set up intersegment mechanism.
		84/07/30.	A. E. Klietz.

	This is the segment mechanism for the back end of a
dual segment program.  

	NOTE: This module must be the first one linked.
*/	

#include <stdio.h>


#define		FRONT_DATA_LEN	0x2500		/* reserved area for front end */
										/* Must match constant in front_segment */
#define		FRONT_NAME		"/sys/scept_child"


	/* Shared data segment */

#asm	"seg 2"		/* Force into constant data segment */
	int		front_pc;		/* pc for front end */
	int		front_cs;		/* code segment of front end */
	
	int		back_pc;		/* pc for back end */
	int		back_cs;		/* code segment for back end */

	char		front_data[FRONT_DATA_LEN];   /* reserve front end data */
	
	
#asm	"seg 1"		/* Force into code segment */
#asm	"sg_ret:	abs	jmp		@ <front_pc>"
#asm	"			export		sg_ret"


main()
{
	int			i;
	char		ds_addr[10];  /* hex string address of ds */

    front_cs = 0;
	tsprintf(ds_addr, "%04x", ds_value());

#ifdef HOME_PC
	create(0, 0x02, 8, 0, 0, FRONT_NAME, ds_addr, 0);
#else
	spawn(FRONT_NAME, ds_addr, 0);
#endif

	for (i=0; i < 10; ++i) { /* wait for front end to get ready */
		sleep(1);
		if (front_cs != 0)
			break; /* it's ready! */
	}

	if (i >= 5) {
		tfprintf(stderr,"Unable to bring up front end.\n");
		exit(-1);
	}
	
	/* Assume that the front end has placed the addr of <main> in
	   to <front_pc> and the front cs in to <front_cs> */


	asm("		mov		ax,cs");	/* set up back_cs */	
	asm("		mov		<back_cs>,ax");

	asm("	abs	jmp		@ <front_pc>");	/* begin main program */
}
	


ds_value()
/* return value of ds register */
{
	asm("	mov		ax,ds");
} /* return value in ax */
