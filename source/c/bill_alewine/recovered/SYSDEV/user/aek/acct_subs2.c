/*		Acct_subs2 - More accounting subroutines.
		83/11/03.	A. E. Klietz.

*/
#define MSGBUFLEN	512
#include "aekdef.h"
#include <stdio.h>
#include "globdef.h"
#include "validef.h"
#include "ac_msgs.h"
#include "appdef.h"
#include "jourdef.h"

extern	struct gl_system_datum *system_data_ptr;    /* pointer to global system data. */
extern	struct gl_user_datum   *user_data_ptr;      /* pointer to global user data. */



do_newapmsg(tid, msg_buf)
/* Procedure do_newapmsg - send new applcation to journal, update tables. */
unsigned	tid;
char		msg_buf[];
{
	struct	ac_newapmsg			newapmsg;
	struct	ac_newapmsg_reply 	newapmsg_reply;
	byte	ttynum;
	unsigned	credit_left, userindex, bindate[2];

	copy(&newapmsg, msg_buf, sizeof(newapmsg));
	ttynum = newapmsg.ac_ttynum;
	if (ttynum == 0 || newapmsg.ac_apnum >= AP_MAXNUM || newapmsg.ac_apnum < 0)
		/* bad newap request */
		newapmsg_reply.ac_apnum_success = 0;
	else  { /* process newap */
		credit_left = credit_update(ttynum, newapmsg.ac_apnum);
		@&user_data_ptr[ttynum].gl_apnum = newapmsg.ac_apnum;
		if (credit_left <= 0)
			newapmsg_reply.ac_apnum_success = 2; /* no credit left */			
		else  	
			newapmsg_reply.ac_apnum_success = 			
				credit_left <= 40 ? 3 : 1;  /* 3 if low, else 1 if ok. */

		write_journal(NEWAP_ENTRY, ttynum, newapmsg.ac_apnum);
		
		}
	acct_reply(tid, &newapmsg_reply, sizeof(newapmsg_reply));
}




do_intlockmsg(tid, msg_buf)
/* Procedure do_intlockmsg - interlock accounting until signal sent. */
unsigned	tid;
char		msg_buf[];
{
	struct	ac_intlockmsg			intlockmsg;
	struct	ac_intlockmsg_reply 	intlockmsg_reply;

	copy(&intlockmsg, msg_buf, sizeof(intlockmsg));
	acct_reply(tid, &intlockmsg_reply, sizeof(intlockmsg_reply));
	await(ACCOUNTING_PORT);  /* block until port signaled */
	read_port(ACCOUNTING_PORT); /* clear any other pending signals */
}


do_readmsg(tid, msg_buf)
/* Procedure do_readmsg - read user's validation file. */
unsigned	tid;
char		msg_buf[];
{
	struct	ac_readmsg			readmsg;
	struct	ac_readmsg_reply 	readmsg_reply;

	copy(&readmsg, msg_buf, sizeof(readmsg));
	readmsg_reply.ac_readmsg_success =
		valid_read(readmsg.ac_userindex, &readmsg_reply.ac_user_record);

	acct_reply(tid, &readmsg_reply, sizeof(readmsg_reply));
}



do_writemsg(tid, msg_buf)
/* Procedure do_writemsg - write a user's validation file. */
unsigned	tid;
char		msg_buf[];
{
	struct	ac_writemsg			writemsg;
	struct	ac_writemsg_reply 	writemsg_reply;

	copy(&writemsg, msg_buf, sizeof(writemsg));
	writemsg_reply.ac_writemsg_success = 
		valid_write(writemsg.ac_user_record.va_userindex,
		&writemsg.ac_user_record);

	acct_reply(tid, &writemsg_reply, sizeof(writemsg_reply));
	write_journal(WRITEVAL_ENTRY, writemsg.ac_ttynum, writemsg.ac_apnum);
}



do_creditmsg(tid, msg_buf)
/* Procedure do_creditmsg - change user's credit balance. */
unsigned	tid;
char		msg_buf[];
{
	struct	ac_creditmsg			creditmsg;
	struct	ac_creditmsg_reply 	creditmsg_reply;
	struct	va_user_record		user_record;
	int		oldcr, addcr;

	copy(&creditmsg, msg_buf, sizeof(creditmsg));

	if (valid_read(creditmsg.ac_userindex,
		&user_record) == 0)
		creditmsg_reply.ac_credit_success = 0; /* failed */
	else {
		oldcr = user_record.va_creditbalance;			
		addcr = creditmsg.ac_credits;
		if (abs(oldcr) + abs(addcr) < max(abs(oldcr), abs(addcr))) {
			creditmsg_reply.ac_credit_success = 2; /* overflow */
			acct_reply(tid, &creditmsg_reply, sizeof(creditmsg_reply));
		}
		else {
			creditmsg_reply.ac_credit_success = 1; /* success */			
			user_record.va_creditbalance = oldcr + addcr;
			if (valid_write(creditmsg.ac_userindex, &user_record) <= 0)
				error("fatal:acct - bad credit validuz write.\n");
			creditmsg_reply.ac_creditbalance = oldcr + addcr;
			acct_reply(tid, &creditmsg_reply, sizeof(creditmsg_reply));
			write_journal(CREDIT_ENTRY, creditmsg.ac_userindex, addcr);
		}
	}
}


write_journal(entry, ttynum, apnum)
/* procedure write_journal - write an entry into the journal file. */
/* if credit entry then ttynum = credit change, apnum = userindex */
int		entry, ttynum, apnum;
{
    FILE	*fp;
    struct	jr_journal_record	journal_record;
	unsigned	bindate[2], index;
	char	name[20];

	journal_record.jr_checkbyte = JR_CHECKBYTE;
	if (entry < 0 || entry >= MAX_ENTRY)
		error("fatal:jour - entry number out of bounds.\n");

	journal_record.jr_entry = entry;
	journal_record.jr_dummy = 0;

	if (entry == CREDIT_ENTRY) { /* credit entry */
		journal_record.jr_credits = apnum; /* apnum = added credits */
		journal_record.jr_apnum = 0;
		journal_record.jr_ttynum = ttynum; /* ttynum = userindex */
		index = ttynum;
	}
	else {	/* normal entry */
		if (apnum < 0 || apnum >= AP_MAXNUM)
			error("fatal:jour - apnum out of bounds!\n");
		journal_record.jr_apnum = apnum;
		journal_record.jr_ttynum = ttynum;
		journal_record.jr_credits   = @&user_data_ptr[ttynum].gl_creditbalance;
		index = @&user_data_ptr[ttynum].gl_userindex;
	}	


	get_date(bindate);
	journal_record.jr_bindate[0] = bindate[0];
	journal_record.jr_bindate[1] = bindate[1];

	tsprintf(name, "/jou/%04u",index);
	if ((fp = fopen(name, "av")) <= 0) {
		tfprintf(stderr, "warn:jour - unable to open %s.\n",name);
		return(-1);
	}
       	
	fput(&journal_record, sizeof(journal_record), fp);

	fclose(fp);
}
