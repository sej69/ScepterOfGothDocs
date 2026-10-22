/*		Mail - mail out billing statements to users.
 		83/11/27.	A. E. Klietz
*/

#include <stdio.h>
#include "aekdef.h"
#include "validef.h"
#include "ac_msgs.h"
#include "jourdef.h"

extern	byte	My_tty;

main()
{

	char	buffer[80];
	struct	va_user_record	user_record;
	int		upper, lower, i;

	if (attach(ACCOUNTING_PORT) != My_tty)
		error("System must be shut down before mailing statements.\n");

	if (attach(16) != My_tty)
		error("Line printer is busy.\n");

    tfprintf(stdout, "Enter lower bound of userindexes? ");
	gets(buffer, BUFLEN);
	lower = atoi(buffer);
	tfprintf(stdout, "Enter upper bound of userindexes? ");
	gets(buffer, BUFLEN);
	upper = atoi(buffer);
	if (lower > upper || lower <= 0 || upper <= 0)
		error("Bad lower or upper bound.\n");
	tfprintf(stdout, "Ready printer and press return.\n");
	gets(buffer, BUFLEN);
	for (i=lower; i <= upper; i++) {
		if (valid_read(i, &user_record) <= 0)
			error("Bad valid read on userindex %04u.\n",i);
		if (user_record.va_userindex == 0)
			tfprintf(stdout, "userindex %04u empty.\n", i);
		else
			print_stmt(i, &user_record);
	}
	tfprintf(stdout, "printing finished.\n");
}



print_stmt(index, user_record)
unsigned	index;
struct		va_user_record		*user_record;
{
	FILE	*lp, *fp;
	char	name[20];
	struct	jr_journal_record	journal_record;
	int		lastcr;
	unsigned bindate[2], day1, day2;
	extern	char	*timeconv(), *dateconv();

	day1 = count_days(user_record->va_lastlogin);
	get_date(bindate);
	day2 = count_days(bindate);
	if (user_record->va_closed || day2 - day1 > 365 || day2 - day1 > 75 &&
		user_record->va_creditbalance == 0) { /* delete account */
		tfprintf(stdout,"Erasing inactive account: %04u.\n",index);
		if (valid_delete(index) == 0)
			error("error deleting account.\n");
		return(0);
	}

	if (user_record->va_nocharge) /* dont mail statement */
		return(0);

	if ((lp = fopen("$lpt","wv")) == 0)
		error("Unable to open $lpt.\n");

	print_header(lp, index, user_record);
	tsprintf(name, "/jou/%04u", index);		
	if ((fp = fopen(name,"rv")) == 0)
		tfprintf(stdout, "%s - empty or unable to open.\n",name);			
	else {
		lastcr = -32767;
		while (fget(&journal_record, 12, fp) == 12) {
			show_journal(lp, &journal_record);
			if (journal_record.jr_entry != CREDIT_ENTRY)
				lastcr = journal_record.jr_credits;
		}   	
   		fclose(fp);
	
		if (lastcr != -32767 && lastcr != user_record->va_creditbalance)
			tfprintf(stdout, "\007validuz: %d cr.  and journal: %d cr. do not balance.\n",
				user_record->va_creditbalance, lastcr);
    	
		tfprintf(lp,"\014");  /* form feed */
	}
	tfprintf(lp,"\n\nCurrent credit balance = %d cr.\n",
			user_record->va_creditbalance, lastcr);
	tfprintf(lp,"Last login was on: %s, %s.\n\n",dateconv(user_record->va_lastlogin),
		timeconv(user_record->va_lastlogin));

	tfprintf(lp,"Current conversion rate is 8 credits per dollar.\n");
	if (user_record->va_creditbalance < 1000) {
		tfprintf(lp,"\n\n To add credits to your account, please mail a check or money order to:\n");
		tfprintf(lp,"          GamBit Multi Systems\n");	
		tfprintf(lp,"          P.O. Box 23495\n");
		tfprintf(lp,"          Minneapolis, MN  55423\n");
		tfprintf(lp,"\n(Be sure to write your codename somewhere on the check.)\n");
	}
	tfprintf(lp,"\n\nThank You!\n");
	tfprintf(lp,"\014");	/* form feed */
	fclose(lp);
}


print_header(lp, index, user_record)
FILE	*lp;
unsigned	index;
struct	va_user_record	*user_record;
{
	char	*skip_string();

	tfprintf(lp,"\n                G a m B i t    M u l t i   S y s t e m s\n");
	tfprintf(lp,"                              Statement of Account\n\n\n");
	tfprintf(lp,"Codename: %s.  Userindex: %04u.\n",
		user_record->va_username, index);
	tfprintf(lp,"\n%s\n%s\n%s\n%s\n\n",
		user_record->va_stringbuf,
		skip_string(1, user_record->va_stringbuf),
		skip_string(2, user_record->va_stringbuf),
		skip_string(3, user_record->va_stringbuf));
}



char *skip_string(count, string)
/* procedure skip_string - skip 'count' strings forward in a list of strings. */
int		count;
char	*string;
{
	register   i;

	for (i=1; i<=count; i++) 
		while (*string++ != '\0')
			;

	return(string);
}
