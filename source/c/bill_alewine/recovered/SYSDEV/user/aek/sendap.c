/*	sendap - send name of application to accounting admin.

	83/11/02.	A. E. Klietz.

	This library routine sends the application number (apnum)
of the current application program to the accounting administrator.
It also checks the reply to see if the user's account has expired.
*/

#include "aekdef.h"
#include <stdio.h>
#include "globdef.h"
#include "appdef.h"
#include "ac_msgs.h"

extern byte  My_tty;
extern unsigned		My_tid;

extern	struct	gl_user_datum	*user_data_ptr;

sendap(apnum)
/* Procedure sendap - send application number to accounting admin. */
int		apnum;
{
	struct	ac_newapmsg		newapmsg;
	struct	ac_newapmsg_reply 	newapmsg_reply;

	if (@&user_data_ptr[My_tty].gl_apnum == AP_SYSTEM)
		return(0);	/* dont change app name */
	newapmsg.ac_checkword = AC_CHECKWORD;
	newapmsg.ac_msgnum = NEWAPMSG;
	newapmsg.ac_ttynum = My_tty;
	newapmsg.ac_apnum = apnum;

	if (send(detach(ACCOUNTING_PORT), &newapmsg, &newapmsg_reply,
		sizeof(newapmsg)) <= 0)
			error("fatal:apnum - Unable to send to Acct Admin.\n");

	switch (newapmsg_reply.ac_apnum_success) {

case 1: break;  /* application change complete. */

case 2: print("Sorry, but your credit has expired for the month.\n");
		print("To renew your account, please mail a check or money order\n");
		print("to:\n\n");
		print("     GamBit Multi Systems, Inc.\n");	
		print("     P.O. Box 23495\n");
		print("     Minneapolis, MN  55423\n\n");
		set_exception(My_tid, 0x0001, 0); /* hangup */
		break;

case 3: print("Note: Your credit account is getting low.\n");
		print("You should renew your subscription soon.\n");
		break;

case 0:
default:
        error("fatal:apnum - unable to change application.\n");

	}

}
