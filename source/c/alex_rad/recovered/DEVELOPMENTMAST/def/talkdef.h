/*		talkdef - talk definitions                    
		83/12/01.	A. E. Klietz

*/

#define TALK_PORT   21

enum us_entrynum {US_ENTERNAME, US_CMD, US_MAXENTRY};

struct	us_user { /* user data specific to talk program. */
	int			us_entry;
	char		us_lowername[12];
	boolean		us_echo;
	unsigned	us_recflags;
	unsigned	us_sendflags;
	int			us_tablenum; /* 0 = main floor */
	char		*us_cmdname;
	boolean		us_master;
	byte		us_yellcount; /* number of yells. */
	unsigned	us_respondflags;
};

struct	tb_table_list {
	char	*tb_name;
	int		tb_public;
	int		tb_locked;
};

#define	TABLE_LIST_LEN 	9

#define CMDLIST_LEN  33
