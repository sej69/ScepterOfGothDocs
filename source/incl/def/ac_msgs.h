/*		ac_msgs.h - Accounting message definitions.
		83/10/21.	A. E. Klietz.
*/
#define AC_CHECKWORD 		0x6C

#ifdef unos
#define LOGINMSG	0
#define LOGOUTMSG	1
#define NEWAPMSG	2
#define LOCKMSG		3
#define READMSG		4
#define WRITEMSG	5
#define CREDITMSG	6
#define QUERYMSG	7
#define FLATMSG		8
#define PARAMMSG	9
#define WHOMSG		10
#define IDMSG		11
#define MAXMSG		12
#else
enum ac_msgnug_type {LOGINMSG, LOGOUTMSG, CONSTMSG,
	NEWAPMSG, LOCKMSG, READMSG, WRITEMSG, CREDITMSG, QUERYMSG,
	FLATMSG, PARAMMSG, WHOMSG, IDMSG, MAXMSG};
#endif

struct ac_generic_reply {   /* generic ac message */
	short		ac_checkword;
	char		ac_buffer[126];
};

struct ac_loginmsg {
	short		ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	char		ac_username[12];
	char		ac_userpw[12];
	int		ac_pid;
	boolean		ac_b1200;	/* True if 1200 baud */
	boolean		ac_b2400;	 /* True if 2400 baud or higher */
};

struct ac_loginmsg_reply {
	short		ac_checkword;
	short		ac_login_success;  /* = 0 if bad pw, =1 if ok. =2 if no credits */
                                /* = 3 if bad login request. 4 if closed, */
				/* = 5 if low on credits. */
	boolean		ac_briefmode;
	boolean		ac_anonymous;
	char		ac_term[20];
	boolean		ac_gender;
	char		ac_realname[BUFLEN];
	boolean		ac_pausemode;
};

struct ac_logoutmsg {
	short		ac_checkword; 
	byte		ac_msgnum;
	byte		ac_ttynum;
};

struct ac_logoutmsg_reply {
	short		ac_checkword;
	short		ac_logout_success;  /* = 0 if bad logout req.  = 1 if ok. */
};

struct ac_newapmsg {
	short		ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	char		ac_apname[BUFLEN];
};

struct ac_newapmsg_reply {
	short		ac_checkword;
	short		ac_apnum_success; /* = 1 ok, 0 if bad call, */
				  /* 2 if out of credits, 3 if low. */
};

struct ac_lockmsg {
	short		ac_checkword;
	byte		ac_msgnum;
	byte		ac_ldummy;
}; 

struct ac_lockmsg_reply {
	short		ac_checkword;
	int		ac_acctpid;
};

struct ac_readmsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_ttynum;
	char			ac_username[12];
};

#ifdef VA_DEFINED
struct ac_readmsg_reply {
	short		ac_checkword;
	short		ac_readmsg_success; /* 0 if bad, userindex if good. */
	char		ac_dummyz[12];
	struct		va_user_record		ac_user_record;
};

struct ac_writemsg {
	short		ac_checkword;
	byte		ac_msgnum;
	byte		ac_ttynum;
	char		ac_username[12];
	struct		va_user_record		ac_user_record;
};

struct ac_writemsg_reply {
	short			ac_checkword;
	short			ac_writemsg_success; /* userindex good, 0 bad */
};
#endif 

struct ac_creditmsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_dumdum;
	char			ac_username[12];
	double			ac_credits;	
	double			ac_free_credits;
	char			ac_credit_comment[BUFLEN];
};

struct ac_creditmsg_reply {
	short			ac_checkword;
	short			ac_credit_success; /* 1 good, 0 bad, 2 neg amt*/
	double			ac_creditbalance;
	double			ac_free_creditbalance;
};


struct ac_querymsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_ttynum;
};

struct ac_querymsg_reply {
	short			ac_checkword;
	double			ac_query_balance;
	double			ac_free_query_balance;
};

struct ac_flatmsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_ttynum;
	byte			ac_flattype; /* 1 = day, 2 = week */
};

struct ac_flatmsg_reply {
	short			ac_checkword;
	short			ac_flat_success; /* 1=good, 0=bad, 2=already */
};

struct ac_idmsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_ttynum;
};

struct ac_idmsg_reply {
	short			ac_checkword;
	short			ac_id_uid;
	short			ac_id_gid;
	int			ac_id_pid;
};

#ifdef GL_DEFINED
struct ac_whomsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_ttynum;
};

struct ac_whomsg_reply {
	short			ac_checkword;
	short			ac_whomsg_success;
	struct gl_user_datum	ac_user_data;
};
#endif

struct ac_parammsg {
	short			ac_checkword;
	byte			ac_msgnum;
	byte			ac_ttynum;
	short			ac_param_success; /* 1 good, 0 bad */
	char			ac_term[20];
	boolean			ac_modify; /* TRUE if modifing values */
	boolean			ac_brief;
	int			ac_adnum;
	boolean			ac_pause;
	boolean			ac_dummy;
	short			ac_uid;   /* read only */
	short			ac_gid;   /* read only */
};
