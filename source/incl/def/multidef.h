/*		Multidef.h - multi task messages.
		83/11/30.	A. E. Klietz.

*/

#define IO_TEXTMSG    0
#define IO_LOGINMSG   1
#define IO_LOGOUTMSG  2
#define IO_RESTARTMSG 3
#define IO_AUTOMSG    4
#define IO_ENDMSG     5
#define IO_PROMPTMSG  6
#define IO_MAXMSG     7

#define IO_CHECKWORD 0x3D
#define	MAXUSERS	32
#define REPLY_LEN 	2000  /* Must be multiple of sizeof(long) */

struct	tx_textline {  /* element of multiuser text chain */
		struct tx_textline   *tx_rlink, *tx_flink;
		unsigned long	tx_notread;	 /* 32 bit field */
		short  		tx_len;		 /* length of text string */
		short		tx_from;	 /* port sending msg */
};


/* These messages are aligned to short integers.  If you need
  a larger alignment pad the messages with dummy fields */

struct io_generic_msg
{
	long		io_mtype;
	unsigned short	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	int		io_length;
	char		io_buffer[300];
};

struct io_textmsg
{
	long		io_mtype;
	unsigned short	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	int		io_length; /* length of message */	
	/*  Message text follows after structure */
};

struct io_loginmsg
{
	long		io_mtype;
	unsigned short	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	unsigned short	io_uid;
	unsigned short	io_gid;
	unsigned	io_pid;
	char		io_name[16];
	boolean		io_gender;
	boolean		io_anonymous;
	boolean		io_brief;
	char		io_realname[BUFLEN];
	byte		io_crtwidth;
};

struct	io_logoutmsg
{
	long		io_mtype;
	unsigned short	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	boolean		io_fullerr;
	byte		io_dummyn;
};

struct	io_restartmsg
{
	long		io_mtype;
	unsigned short	io_checkword;
        byte		io_msgnum;
	byte		io_ttynum;
};

struct	io_automsg
{
	long		io_mtype;
	unsigned short	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	boolean		io_toggle; /* TRUE if entering auto, FALSE if leaving*/
	byte		io_dummyx;
};

struct	io_endmsg
{
	long		io_mtype;
	unsigned short	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	boolean		io_abort;
	byte		io_dummyq;
};

struct  io_promptmsg
{
	long		io_mtype;
	unsigned short 	io_checkword;
	byte		io_msgnum;
	byte		io_ttynum;
	char		io_prompt[BUFLEN];
};
