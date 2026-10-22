/*		maildef - mail definitions
		83/12/23.	A. E. Klietz
    
*/

#define MAXSUBSCRIPTIONS  12
#define BUFLINES 25
/* 25 lines, 80 characters (BUFLEN) per line. */

enum tp_access_level {TP_NONE, TP_READ, TP_WRITE, TP_READWRITE};

struct subscribe_list_type {
	char	topic_name[12];
	int		topic_highread; /* highest read note. */
};

/* max number of subscriptions.  topic.tp_subscribe_list[0] = user's
   own name. */

struct tp_topic {  /* this is the format of a topic header file. */
	char	tp_name[12];	/* topic name. */
	char    tp_Name[12];    /* upper case name. */
	char	tp_notice[80];	/* user's notice message. */
	int		tp_access;      /* access permission: read, write, none. */
	int		tp_lonum;		/* lower bound of notes.  0 = no notes. */
	int		tp_hinum;		/* high bound of notes.  0 = no notes. */
	int		tp_maxnotes;	/* max number of notes before scrolling. */
	boolean	tp_private;		/* true if private mailbox */
	byte	tp_dummy1;
	struct  subscribe_list_type  tp_subscribe_list[MAXSUBSCRIPTIONS];
							/* list of selected topics to scan. */
};

