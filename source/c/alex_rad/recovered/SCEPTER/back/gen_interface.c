/*		build_interface.c - generate interface table
    						from load map.
    	84/07/30.	A. E. Klietz.
*/

#include <stdio.h>

main()
{
	char	buf[255];
	char	name[80];
	int		addr, segnum, sg_ret;

	while (!feof(stdin)) {
		fgetstr(stdin, buf, 80);
		sg_ret = 0;
		tsscanf(buf, "<%d> %04x %s", &segnum, &sg_ret, name);
		if (segnum == 1 && equal(name, "sg_ret"))
			break;
	}

	if (sg_ret == 0) {
		tfprintf(stderr, "sg_ret not found in map.\n");
		exit(-1);
	}

	tfprintf(stdout, "#asm	\"seg 2\"\n");
	tfprintf(stdout, "#asm	\"sg_ret:\"\n");
	tfprintf(stdout, "#asm	\" w		data	%05xh\"\n\n", sg_ret);
	rewind(stdin);

	tfprintf(stdout, "#asm	\"seg 1\"\n\n");

	tfprintf(stdout, "#asm	\"long_jump: pop		ax\"\n");
	tfprintf(stdout, "#asm	\"		mov		<front_pc>,ax\"\n");
	tfprintf(stdout, "#asm	\"		push	sg_ret\"\n");		
	tfprintf(stdout, "#asm	\"	abs	jmp		@ <back_pc>\"\n");

	while (!feof(stdin)) {
		fgetstr(stdin, buf, 80);
		segnum = 0;
		tsscanf(buf, "<%d> %04x %s", &segnum, &addr, name);
		if (segnum == 1 && !equal(name, "<main>") && !equal(name, "<exit>")
				&& name[0] == '<') {
			tfprintf(stdout, "#asm	\"%s:	mov		<back_pc>,#%05xh\"\n", name, addr);
			tfprintf(stdout, "#asm	\"		jmp		long_jump\"\n");
			tfprintf(stdout, "#asm	\"		export	%s\"\n\n", name);
		}
		else
			if (segnum == 2 || segnum == 3) {
				tfprintf(stdout, "#asm	\"		seg		%d\"\n", segnum);
				tfprintf(stdout, "#asm	\"%s	equ		%05xh\"\n", name, addr);
				tfprintf(stdout, "#asm	\"		export	%s\"\n\n", name);
				tfprintf(stdout, "#asm	\"		seg		1\"\n");
			}
	}
	exit(0);
}
