/*		Combat - Multi-user spaceship combat.
		84/05/06.	A. E. Klietz

		Copyright (c) 1984 by GamBit Multi Systems, Inc.
							  All Rights Reserved.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"

#define COMBAT_PORT		19

extern	struct	tx_textline		*Text_header;

main()
{
	unsigned	tid;
	struct		io_generic_msg	io_msg;
	struct		io_endmsg		endmsg;
	word		count = 0;
	TI_TIME		bindate;
	boolean		initial_flag = TRUE;
	

	unbreakable();
	initialize();
	endmsg.io_checkword	= IO_CHECKWORD;
	endmsg.io_msgnum	= IO_ENDMSG;
	
	for (;;) {
		if (All_mask == 0 && !initial_flag) /* if no active users */
			exit(0);

		breakable();
		if ((tid = receive(0, &io_msg, sizeof(io_msg))) == 0) {
			errlog("main - bad receive.\n");
			continue;
		}

		unbreakable();
		initial_flag = FALSE;
		if (++count % 10 == 0)
			check_hangup();
		update(); /* update the ship positions */

		if (io_msg.io_checkword != IO_CHECKWORD) {
			errlog("main - bad checkword.\n");
			continue;
		}
		endmsg.io_abort = FALSE;
		switch (io_msg.io_msgnum) {

case	IO_TEXTMSG:		if (do_textmsg(&io_msg) < 0)
							endmsg.io_abort = TRUE;
						break;

case	IO_RESTARTMSG:  if (do_restartmsg(&io_msg) < 0)
							endmsg.io_abort = TRUE;
						break;

case	IO_LOGINMSG:	check_hangup();
						count = 0;
						do_loginmsg(&io_msg, tid);
						break;

default:		errlog("main - bad io_msgnum.\n");
				continue;

		}
		endmsg.io_ttynum = io_msg.io_ttynum;
		reply(tid, &endmsg, sizeof(endmsg));
	}
}



initialize()
{
	register i;

	setup("combat");
	if (attach(COMBAT_PORT) != 0)
		crash("initial - combat program already active!\n");
	randomize(SYSTEM_SEED); /* set random number generator. */
	Text_header = (struct tx_textline *) alloc(sizeof(struct tx_textline));
	Text_header->tx_rlink = Text_header;
	Text_header->tx_flink = Text_header;
	for (i=0; i<MAX_PORTS; i++)
		genuser[i].gn_ttynum = -1; /* initialize ttynums to -1 */
	do_initialize();
	errlog("Task initialized.\n");
}



do_loginmsg(loginmsg, tid)
struct	io_loginmsg	*loginmsg;
unsigned			tid;
{
    register	i;
	struct	io_logoutmsg	logoutmsg;
	struct	gn_genuser		*gnptr;
	char	*name;
	char	buffer[BUFLEN];
	
	for (i=0; i<MAX_PORTS && genuser[i].gn_userindex != 0; ++i)
		;
	if (i >= MAX_PORTS) { /* task full */
		logoutmsg.io_checkword = IO_CHECKWORD;
		logoutmsg.io_msgnum	   = IO_LOGOUTMSG;
		logoutmsg.io_ttynum	   = loginmsg->io_ttynum;
		logoutmsg.io_fullerr   = TRUE;
		write_msg(tid, &logoutmsg, sizeof(logoutmsg));
		return(0);
	}
	gnptr = &genuser[i];
	My_port = i;  /* set global masks and port vars */
	user = &user_array[i];
	My_mask = 1 << My_port;
	All_mask |= My_mask;
	if (loginmsg->io_useracct == SUPER_ACCT)
		Staff_mask |= My_mask;
	Allbutme_mask = All_mask & ~My_mask;
	strcpy(gnptr->gn_name, "<login>");
	gnptr->gn_ttynum = loginmsg->io_ttynum;
	gnptr->gn_usernum = loginmsg->io_usernum;
	gnptr->gn_userindex = loginmsg->io_userindex;
	gnptr->gn_useracct  = loginmsg->io_useracct;
	gnptr->gn_tid		= loginmsg->io_tid;
	gnptr->gn_waitmask	= 0;
	mwrite(My_mask, "Space Combat Ver 1.1  (small ship)\n");
	mwrite(My_mask,"\nYou climb into the cockpit of your GMB-01X.\n");
	strcpy(buffer, get_tty_login_name(gnptr->gn_ttynum));
	mwrite(My_mask, "Please sign your name in the flight log (%s)",
		buffer);
	user->us_entry = US_ENTERNAME;
	write_text(My_port);
}


int do_restartmsg(restartmsg)
struct	io_restartmsg	*restartmsg;
{
	if ((My_port = get_port(restartmsg->io_ttynum)) < 0)
		return(My_port); /* if port not active */
	My_mask = 1 << My_port;
	Allbutme_mask = All_mask & ~My_mask;

	write_text(My_port); /* find and send text, adding restart if needed */
	return(My_port);
}


int do_textmsg(textmsg)
struct	io_textmsg		*textmsg;
{
	char	*text;
	register i;

	if ((My_port = get_port(textmsg->io_ttynum)) <0)
		return(My_port);	/* if port not active */
	My_mask = 1 << My_port;
	Allbutme_mask = All_mask & ~My_mask;
	user = &user_array[My_port];
	text = &textmsg->io_start;	/* get text line */	
	if (genuser[My_port].gn_logout) { /* if logout requested */
		write_text(My_port); /* write_text will process logout */
		return(My_port);
	}
	do_input(text);
	write_text(My_port); /* find and send text, adding restart if needed */
	return(My_port);
}
    


do_logout(port)
int		port;
{
	unsigned	mask;
	int			i;
	struct		gn_genuser		*gnptr;

	gnptr = &genuser[port];
	mask = 1 << port;
	gnptr->gn_ttynum = -1;
	gnptr->gn_usernum = 0;
	gnptr->gn_userindex = 0;
	gnptr->gn_useracct = 0;
	gnptr->gn_logout = FALSE;
	gnptr->gn_tid	= 0;
	gnptr->gn_waitmask = 0;
	All_mask &= ~mask;
	Staff_mask &= ~mask;
}



check_hangup()
{
	register  i;
	unsigned_word	index;
	struct	gn_genuser	*gnptr;

	for (i=0; i < MAX_PORTS; i++) {

		gnptr = &genuser[i];
		if (gnptr->gn_userindex != 0)
			if (get_tty_pid(gnptr->gn_ttynum) != gnptr->gn_tid) {
/* user hung up the phone, or /sys/io task died. */
				All_mask &= ~(1 << i);
				Allbutme_mask = All_mask;
				erasetext(i);
				My_mask = 0;
				user = &user_array[i];
				My_port = i;
				do_hangup();
				do_logout(i);
			}
		
	}
}
