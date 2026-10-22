/*		monitor - Central System Terminal Monitor.
		83/09/24.	A. E. Klietz.

		This program monitors the activity on all dial-up terminals.
it sleeps for MONSLEEP_SECS seconds and wakes up to see if any
terminal tasks have died due to hangup or fatal program error. When
the monitor discovers a terminal whose taskid reverts back to IDLE,
it forces the modem to hang up the phone by zeroing the DTR bit in the
Modem Control Register.  It also resets the terminal's default 
characteristics and sends a message to the accounting administrator
saying the user logged out.

		Another task of the monitor is to check for no carrier.  If
a terminal loses it's carrier, then the monitor issues a hangup_exception
to the task phone by zeroing the DTR bit in the Modem Control Register
for two seconds.

		When the monitor detects a new carrier, it escapes a GamBit shell
to handle the login procedures.

		The monitor also resets the gl_monitor byte for all other ttys
when a tty that has been monitoring other ttys hangs up.

		The monitor also updates the peak/nonpeak flag.
*/

#include <stdio.h>
#include <io.h>
#include <dev.h>
#include "/def/aekdef.h"
#include "/def/termdef.h"
#include "/def/ac_msgs.h"
#include "/def/globdef.h"
#include "/def/appdef.h"
#define  EXC_HANGUP		0x0001
#define  EXC_BREAK      0x0002
#define  RUN_BACKGROUND 0x0001

/* flag for setting debug mode. */
/* #define DEBUG */

extern unsigned		My_tid, Dads_tid;
extern char My_tty, My_priority;

struct	gl_user_datum		*user_data_ptr;
struct	gl_system_datum		*system_data_ptr;

main(argc, argv) 
int		argc;
char	*argv[];

{
	char	*version = "1.4";
	int		i, loopcount;
	unsigned taskid; /* id of current running task on device. */
    struct termlist_type	*term;
	unsigned msreg; /* modem status register port. */
	FILE	*fp;

	sleep(2); /* wait so we dont mix output with foreground task. */

	tprintf("\nCentral Terminal Monitor ver %s.\n",version);
	
	if (argc == 0) {  /* query. */
		tfprintf(stderr,"monitor &  -  initiate terminal monitor.\n");
		exit(0);
	}

	if	(My_tid == tty_tid(My_tty))
		abort("fatal:monitor - The monitor must run as a background task.\n");

	if (attach(MONITOR_PORT) != 0)
		abort("fatal:monitor - Another monitor task is already active!\n");

/* The following is the initialization loop for the monitor. */

	for (i=0; i < TERMLIST_LEN; i++)
		if (termlist[i].enabled) {
			term = &termlist[i];
			term->taskid = tty_tid(term->ttynum);
			if (term->taskid == -1)  /* what? */
				error("fatal:monitor - nonexistant tty: %d.\n",term->ttynum);    
			if (term->taskid == IDLE)  {
				init_terminal(term);
				setup_stty(term);

			}
		}

		set_glob(); /* set global pointers. */
		check_timeout();  /* set up peak/nonpeak flag */
		tprintf("The GamBit Timeshare System is ready for operations on %s, %s.\n",
			date(), time());

/* The following is the main loop for the monitor. */

	while (TRUE) { /* loop forever. */
		for (i = 0; i < TERMLIST_LEN; i++)
			if (termlist[i].enabled) {  /* if disabled then ignore port. */
				term = &termlist[i];
				taskid = tty_tid(term->ttynum);
				if ((taskid == IDLE) && (term->taskid != IDLE)) { /* do hangup processing. */
					term->taskid = IDLE;
					send_accounting_logoff(term);
					setup_stty(term); /* set up login config. */
					tprintf("%s, %s info:monitor - no task for tty%d.\n",
						date(), time(), term->ttynum);
				} /* idle */
				else
					term->taskid = taskid;
				if (term->iobase != 0) /* check carrier change. */
					if ((msreg = io_in(term->iobase+6)) & 0x0008)
						if ((msreg & 0x0080) && (taskid == IDLE)) { /* connected. */
                            if (@&system_data_ptr->gl_sysidle) {
tfprintf((fp = fopen(term->name,"wv")),"idle:monitor - system is down.\n");
								fclose(fp);
							}
							else {

/* Start up the GamBit shell so the user can log in to the system. */

								tprintf("%s, %s info:monitor - connection on tty%d.\n",
									date(), time(), term->ttynum);
									escape_tty(term->ttynum, "/cmds/sh");
							}
						}
						else { /* hangup. */
							/* set hangup and thus kill task. */
							set_exception(taskid, EXC_HANGUP, 0);
							tprintf("%s, %s info:monitor - hangup on tty%d.\n",    
								date(), time(), term->ttynum);
						}
			} /* scan */
			if (loopcount >= 20) { /* time to check for timeouts. */
				loopcount = 0;
				check_timeout();
			}
			loopcount++;
			breakable(); /* allow breaks while sleeping. */
			sleep(MONSLEEP_SECS);  /* go to sleep for a few seconds. */
			unbreakable();  /* no interrupts now! */
	} /* loop forever */

	abort("fatal:monitor - unexpected termination.\n");
}

setup_stty(term)
/* Procedure setup_stty - reset the modem to a login configuration. 
	This procedure will set the attributes of the terminal as follows:

		1) Change the tty_RUBOUT to a backspace (0x0008)
		2) Change the line-delete character to control-u.
		3) Disable input gating.
		4) Set full duplex, XON/XOFF, line editing, and tab expanding.
        5) Disconnect phone for modem devices.
		6) set 300 baud 8 bits, 1 stop bit, no parity.
*/

struct termlist_type *term;

{
	FILE *fp;
	struct dev_entry buffer;   /* attribute block buffer. */
	unsigned	mcreg; /* modem control register port */

#ifdef DEBUG
	tprintf("debug:monitor - Device attribute set: %s.\n",term->name);
#endif

	fp = fopen(term->name,"wv");
	if (fp == 0) {  /* unable to access modem. */
		tfprintf(stderr,"warn:monitor - Unable to open tty%d.\n",term->ttynum);
		return;
	}
	get_attr(fp, &buffer);
	if (buffer.tty_type == DEV_ASCII) 
		buffer.tty_RUBOUT = 0x0008; /* change delete to backspace. */
	else
		buffer.tty_RUBOUT = 0x007F; /* delete key. */
	buffer.tty_CANCEL = 0x0015; /* change line-erase to cntl-u. */
	set_attr(fp, &buffer); /* set attributes */
	set_option(fp, get_option(fp) & ~IGATE | ECHO | EDIT | XON_XOFF | ETAB);

	fclose(fp);
	if (term->iobase != 0) {  /* if dial-up port */
		/* force hangup by resetting DTR bit on modem. */
		mcreg = io_in(term->iobase+4);
		io_out(term->iobase+4,mcreg & ~0x0001);
		sleep(2); /* wait for phone to disconnect. */
		io_out(term->iobase+3, 0x83); /* 8 bits, no parity, access baud rate. */
		io_out(term->iobase+0, 0x80); /* lower byte, 300 baud. */
		io_out(term->iobase+1, 0x01); /* upper byte, 300 baud. */
		io_out(term->iobase+3, 0x03); /* deaccess baud rate */
		io_out(term->iobase+4,mcreg);
	}
}

init_terminal(term)                   
/* Procedure init_terminal - initialize port at boot-up.   
This procedure will flush the input and output buffers, and 
set the shell-escape character.  For modems, it also sends
the necessary initialization codes for auto-answering. */

struct termlist_type *term;

{
	FILE *fw, *fr;
	struct	dev_entry	buffer;

	fw = fopen(term->name, "wv");
	fr = fopen(term->name, "rv");
	if (fr == 0 || fw == 0) { /* unable to open modem. */
		tfprintf(stderr,"warn:monitor - Unable to init tty%d.\n",term->ttynum);
		return(0);
	}

/* Before we can initialize the modem, we have to disable echoing, editing
and input gating on the terminal so we don't choke the monitor on an
i/o block.  (argh!)     set_stty will reset the parameters correctly. */

	get_attr(fw,&buffer);
	if (buffer.tty_type == DEV_ASCII) {
		buffer.tty_ESCAPE = 0x0000; /* disable shell escape. */
		term->iobase = buffer.tty_ioport;
	}
	else {
		buffer.tty_ESCAPE = 0x001A; /* reset escape to cntl-z */
		term->iobase = 0;
	}
	set_attr(fw,&buffer);
	set_option(fw, get_option(fr) & ~ECHO & ~EDIT);
	tfprintf(fw,"%cAT M0E0Q1S0=1S10=10S2=21%c\n",CR,CR); /* initialize Hayes modem. */
	fflush(fw);             
	sleep(5); /* give modem a chance to initialize. */
	while (char_waiting(fr))
		getc(fr); /* flush input buffer */
	fclose(fw);
	fclose(fr);
	tprintf("info:monitor - %s initialized.\n",term->name);
}


send_accounting_logoff(term)
/* Procedure send_accounting_logoff - send a message to the
	Accounting Administrator saying the terminal hung up. */

struct termlist_type *term;

{
    struct	ac_logoutmsg	logoutmsg;
	struct	ac_logoutmsg_reply	logoutmsg_reply;

	logoutmsg.ac_checkword = AC_CHECKWORD;
	logoutmsg.ac_msgnum	  = LOGOUTMSG;
	logoutmsg.ac_ttynum	  = term->ttynum;

	if (send(detach(ACCOUNTING_PORT), &logoutmsg, &logoutmsg_reply,
			sizeof(logoutmsg)) == 0)
		error("fatal:monitor - unable to send to accounting admin.\n");

	if (logoutmsg_reply.ac_logout_success != 1) /* bad logout. */
		tprintf("note:monitor - bad logout request for tty%d.\n",term->ttynum);

}



check_timeout()
/* procedure check_timeout - check for 15 minute timeout. */
/* Also, check for peak/non-peak time and set flag as needed. */
{
	register  i;
	struct	gl_user_datum  *user_ptr;
	unsigned peak, olddate[2], bindate[2];
	boolean	peakchange;
	extern unsigned secdiff();

	get_date(bindate);
	peak = bindate[1] / 3600;
	if (bindate[0] & 0x01)
		peak += 12; /* 24 hour time format */
	peak = (peak < 2 || peak >= 12);  /* true if peak time. */
	peakchange = (peak != @&system_data_ptr->gl_peakflag);
	
	for(i=0; i < TERMLIST_LEN; i++)
		if (termlist[i].enabled) {
			user_ptr = &user_data_ptr[termlist[i].ttynum];
			if (@&user_ptr->gl_userindex != 0 &&  /* must be logged in */
					@&user_ptr->gl_apnum != AP_SYSTEM) { /* dont check system tasks*/
				if (peakchange)
					credit_update(termlist[i].ttynum, 0); 
				/* update credits before changing peak flag */
				olddate[0] = @&user_ptr->gl_binarylastcr[0];
				olddate[1] = @&user_ptr->gl_binarylastcr[1];
				if (secdiff(olddate, bindate) > 900) { /* fifteen minutes */
					set_exception(termlist[i].taskid, EXC_HANGUP, 0);
					tprintf("%s %s info:monitor - timeout on tty%d.\n",
						date(), time(), termlist[i].ttynum);
				}
			}
        }
	if (peakchange) {
		@&system_data_ptr->gl_peakflag = peak;
		print("%s, %s info:monitor - start of %speak time.\n",
			date(), time(), (peak ? "" : "non-"));
	}

	get_date(olddate); /* set olddate to 4:40 am */
	olddate[0] &= ~0x01; /* force am */
	olddate[1] = 4*60*60 + 40*60; /* 4:40 */
	if (abs(mindiff(olddate, bindate)) <= 5 && !@&system_data_ptr->gl_warncrash) {
		@&system_data_ptr->gl_warncrash = TRUE;
		create(0, RUN_BACKGROUND, My_priority, 0, 0, "/cmds/qnxsh", "/config/down_script",
			0); /* create script to shut down system in 20 minutes. */
	}
}


		

tprintf(format, a1, a2, a3, a4, a5, a6)
/* Procedure tprintf - short form of tfprintf. */

char *format;
int a1, a2, a3, a4, a5, a6;
{
	tfprintf(stdout, format, a1, a2, a3, a4, a5, a6);
}

