#include "std.h"

#ifndef MAXUSERS
#define	MAXUSERS	32
#endif

struct ui_userinfo { /* multi user info record */
		char		ui_name[16];
		long		ui_lastflush; /* Time of last output */
		unsigned int	ui_waitmask;  /* Mask of users being waited on
						 while in auto mode. */
		unsigned short  ui_uid;     /* User id */
		unsigned short  ui_gid;     /* Group id */
		unsigned 	ui_pid;     /* Process id for io program */
		byte		ui_ttynum;  /* tty number */
		boolean		ui_logout;  /* Set TRUE by logout() */
		boolean		ui_dummy2;  /* dummy fill */
		boolean		ui_anonymous; /* TRUE if user is anonymous */
		boolean		ui_brief;  /* TRUE if brief mode */
		boolean		ui_auto;   /* TRUE if in auto mode */
		boolean		ui_autoreq;/* TRUE if requested auto mode */
		unsigned byte	ui_crtwidth; /* Width of CRT. */
		char		ui_realname[40]; /* Full real name */
		unsigned int	ui_monmask;  /* Mask of mon-ing ports */
		char		ui_dummy[36]; /* Dummy space */
};

extern  unsigned int	My_mask, All_mask, Allbutme_mask;
extern	int		My_port;

extern	struct	ui_userinfo	userinfo[MAXUSERS];
