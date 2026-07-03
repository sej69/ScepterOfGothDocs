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
equal(name, "<exit>")
				&& name[0] == '<') {
			tfprintf(stdout, "#asm	\"%s:	mov		<back_pc>,#%05xh\"\n", name, addr);
			tfprintf(stdout, "#asm	\"		jmp		long_jump\"\n");
			tfprintf(stdout, "#asm	\"		export	%s\"\n\n", name);
