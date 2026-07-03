#define SH_NOTHING	0
#define	SH_SEND		41	
#define	SH_RECEIVE	13

#define SH_CHECKWORD	0x12345678

struct mt_mtable {
	long	mt_checkword;
	char	*mt_buf;
	int	mt_pid;
	short   mt_command;
	short	mt_buflen;
	short	mt_to;
	char	*mt_localbuf;
};

struct sh_share {
	long	sh_checkword;
	char    *sh_buf; /* physical address, prepended with checkword */
	int	sh_pid;	
	short	sh_command;
	short	sh_buflen;
	short	sh_to;
	boolean	sh_repeat; /* TRUE if mio asks proc to repeat */
	boolean	sh_success;	
	short	sh_from;
	int	sh_frompid;
};
