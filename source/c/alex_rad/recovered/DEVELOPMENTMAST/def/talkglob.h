/*		talkdef - talk global vars.
		83/12/01.	A. E. Klietz

		This file is dependent on multidef.h and talkdef.h.

*/

struct	gl_user_datum	*user_data_ptr;
struct	gl_system_datum	*system_data_ptr;

extern	unsigned	My_tid;

int		My_port;
unsigned My_mask, All_mask, Allbutme_mask, Staff_mask, Local_mask;
char	Task_name[12];  /* name of task */

struct	tx_textline	*Text_header;   /* text output circular buffer */

struct	gn_genuser genuser[MAX_PORTS];  /* general purpose user table. */

struct	us_user		 *user, user_array[MAX_PORTS];

struct	tb_table_list table_list[] = {
	"reception hall", TRUE, FALSE,
	"conf table",   TRUE, FALSE,
	"debate table", TRUE, FALSE,
	"gaming table", TRUE, FALSE,
	"social table", TRUE, FALSE,
	"", FALSE, FALSE,
	"", FALSE, FALSE,
	"", FALSE, FALSE,
    "", FALSE, FALSE,
};    

struct cmdlist_type cmdlist[] = {
"auto",		  20,
"block",       1,
"broadcast",   2,
"channel",     3,
"echo",        4,
"end",         5,
"exclude",     6,
"exit",        5,
"flash",       7,
"help",		  22,
"include",     8,
"leave",       9,
"list",       10,
"master",     11,
"moderator",  11,
"nosend",     12,
"private",	  18,
"public",	  19,
"quit",        5,
"receive",    13,
"remove",     14,
"respond",    23,
"scan",		  17,
"send",       15,
"sit",		   3,
"status",     16,
"table",       3,
"users",	  17,
"wait",       21,
"where",      17,
"whisper",     7,
"who",        16,
"yell",		  24,
};
