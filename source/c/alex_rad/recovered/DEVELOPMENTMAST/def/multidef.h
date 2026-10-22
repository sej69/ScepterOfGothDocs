/*		Multidef.h - multi task messages.
		83/11/30.	A. E. Klietz.

	This file is dependent on aekdef.h.

*/

enum io_msg_type {IO_TEXTMSG, IO_LOGINMSG, IO_LOGOUTMSG, 
	IO_RESTARTMSG, IO_AUTOMSG, IO_ENDMSG, IO_MAXMSG};

#define IO_CHECKWORD 0x3D
#define	MAX_PORTS	16
#define REPLY_LEN   1024

struct	tx_textline {  /* element of multiuser text chain */
		struct	tx_textline		*tx_rlink, *tx_flink;
		unsigned	tx_from;	 /* port sending msg */
		int     	tx_notread;  /* 16 bits, 1 for each unread port. */
		unsigned	tx_len;		 /* length of text string */
		char		tx_start;	 /* first character of text string. */
};

struct gn_genuser { /* general multi user record */
		char	gn_name[12];
		byte	gn_ttynum;
		byte	gn_usernum;
		unsigned gn_userindex;  /* =0 if not active */
		boolean gn_logout;
		byte	gn_useracct;
		unsigned gn_tid;
		unsigned gn_waitmask;
};


struct io_generic_msg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
	int		io_length;
	char	io_buffer[300];
};

struct io_textmsg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
	int		io_length; /* length of message */	
	char	io_start;  /* first character of message */
};

struct io_loginmsg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
	byte	io_usernum;
	byte	io_useracct;
	unsigned io_userindex;
	unsigned io_tid;
};

struct	io_logoutmsg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
	boolean	io_fullerr;
};

struct	io_restartmsg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
};

struct	io_automsg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
	boolean io_toggle; /* TRUE if entering auto, FALSE if leaving */
	byte	io_dummya;
};

struct	io_endmsg
{
	int		io_checkword;
	byte	io_msgnum;
	byte	io_ttynum;
	byte	io_abort;
};
