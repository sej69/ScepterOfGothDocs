/*	kmem.h - Unos kernel memory definitions.
	84/12/28. A. E. Klietz
*/

#define	PS_SIZE		0x14A	/* Size of ps record */

#define RUN		0x01
#define RDY		0x02
#define WAIT		0x03
#define STOP		0x04
#define	DEAD		0x05
#define EXIT		0x06

struct	ps_psrec {
		long		ps_qleft, ps_qright;
		long		ps_pid;
		short		ps_flags;
		byte		ps_state;
		byte		ps_stateaddtl;
		short		ps_dummy1[7];
		short		ps_cntlterm;
		long		ps_pgrpid;
		short		ps_dummy2[10];
		long		ps_parent;
		short		ps_dummy3[4];
		short		ps_euid;
		short		ps_egid;
		short		ps_uid;
		short		ps_gid;
		short		ps_dummy4[10];
		long		ps_textlen, ps_datalen, ps_stacklen;
		long		ps_dummy5;
		char		*ps_textaddr, *ps_dataaddr, *ps_stackaddr;
		short		ps_dummy6[58];
		byte		ps_lowpri; /* These are wrong.. */
		byte		ps_dummy7;
		byte		ps_curpri;
		byte		ps_dummy8;
		byte		ps_highpri;
		byte		ps_dummy9;
		short		ps_dummy10[42];
};

#define NULL_PS	((struct ps_psrec *) 0)
