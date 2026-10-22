/*		ac_msgs.h - Accounting message definitions.
		83/10/21.	A. E. Klietz.

	The following is a list of standard message definitions for the
accounting administrator (/sys/accounting).

	This file is dependent on aekdef.h and globdef.h.

*/

#define AC_CHECKWORD 		0x6C
#define	ACCOUNTING_PORT		22

enum ac_msgnug_type {GLOBMSG, LOGINMSG, LOGOUTMSG, CONSTMSG,
	NEWAPMSG, INTLOCKMSG, READMSG, WRITEMSG, CREDITMSG, ACMAXMSG};

struct ac_generic_reply {   /* generic ac message */
	int			ac_checkword;
	char		ac_buffer[126];
};

struct ac_globmsg {
	int			ac_checkword;
	byte		ac_msgnum;
};	

struct ac_globmsg_reply {
	int			ac_checkword;
	unsigned	ac_global_seg_addr;
	struct user_datum	*ac_user_data_ptr;		/* pointer to global user data array. */
	struct system_datum	*ac_system_data_ptr; 	/* pointer to global system data. */
};

struct ac_loginmsg {
	int			ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	char		ac_username[12];
	char		ac_userpw[12];
	char		ac_ttyname[8];
};

struct ac_loginmsg_reply {
	int			ac_checkword;
	byte		ac_login_success;  /* = 0 if bad pw, =1 if ok. =2 if no credits */
};                              /* = 3 if bad login request. 4 if closed, */
								/* = 5 if low on credits. */
struct ac_logoutmsg {
	int			ac_checkword; 
	byte		ac_msgnum;
	byte		ac_ttynum;
};

struct ac_logoutmsg_reply {
	int			ac_checkword;
	byte		ac_logout_success;  /* = 0 if bad logout req.  = 1 if ok. */
};

struct ac_constmsg {
	int			ac_checkword;
	byte		ac_msgnum;
};

struct ac_constmsg_reply {
	int			ac_checkword;
	char		ac_sysnam[12];
	char		ac_sysloc[12];
};

struct ac_newapmsg {
	int			ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	byte		ac_apnum;
};

struct ac_newapmsg_reply {
	int			ac_checkword;
	int			ac_apnum_success; /* = 1 ok, 0 if bad call, */
								  /* 2 if out of credits, 3 if low. */
};

struct ac_intlockmsg {
	int			ac_checkword;
	byte		ac_msgnum;
}; 

struct ac_intlockmsg_reply {
	int			ac_checkword;
};

#ifdef	va_defined

/* define read/write only if validation records are defined */

struct ac_readmsg {
	int			ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	unsigned	ac_userindex;
};

struct ac_readmsg_reply {
	int			ac_checkword;
	int			ac_readmsg_success; /* 0 if bad, userindex if good. */
	int 		ac_dummyr;			/* dummy fill */
	struct		va_user_record		ac_user_record;
};

struct ac_writemsg {
	int			ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	byte		ac_apnum;
	byte		ac_dummyw;
	struct		va_user_record		ac_user_record;
};

struct ac_writemsg_reply {
	int			ac_checkword;
	int			ac_writemsg_success; /* userindex good, 0 bad */
};

#endif


struct ac_creditmsg {
	int			ac_checkword;
	byte		ac_msgnum;
	byte		ac_dummycr;
	unsigned	ac_userindex;
	int			ac_credits;	
};

struct ac_creditmsg_reply {
	int			ac_checkword;
	int			ac_credit_success; /* 1 good, 0 bad, 2 overflow */
	int			ac_creditbalance;
};
