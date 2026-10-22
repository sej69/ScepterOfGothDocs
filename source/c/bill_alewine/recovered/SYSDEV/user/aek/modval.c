/*		modval - modify validation file.
		83/10/10.  A. E. Klietz.

		This program handles all modifications of the validation file
(/val/validuz) outside of normal accounting.

*/
#include <stdio.h>
#include "aekdef.h"
#include <dev.h>
#include "globdef.h"
#include "validef.h"
#include "ac_msgs.h"

#define EXC_BREAK 2

extern  byte User_num;

struct	gl_user_datum	*user_data_ptr;
struct	gl_system_datum	*system_data_ptr;

static struct cmdlist_type cmdlist[] = {
	"create", 1,
	"end", 2,
	"exit", 2,
	"help", 4,
	"list", 5,
	"modify", 6,
	"receive", 7,
	"show", 8
};

#define CMDLIST_LEN (sizeof(cmdlist) / sizeof(struct cmdlist_type))

static struct cmdlist_type modlist[] = {
	"account", 3,
	"anonymous",15,
	"birthdate", 9,
	"charge", 12,
	"codename", 1,
	"control-h", 6,
	"credit", 7,
	"echo", 4,
	"edel", 5,
	"linedel", 11,
	"mailaddr", 13,
	"open", 8,
	"password", 2,
	"rating", 10,
	"state_resident",14
};

#define MODLIST_LEN (sizeof(modlist) / sizeof(struct cmdlist_type))

main()
{
BUFTYPE buffer;
int		len;
FILE	*fp;
extern	break_handler();

	if (User_num != -1)
		error("fatal:modval - illegal user access.\n");

	unbreakable(); /* disable breaks here. */
	set_glob();    /* set global pointers */
	 
	print(" \n");
	
	exc_handler(EXC_BREAK, 0, break_handler);
    if(!setexit()) {
        len = char_waiting(stdin);
        if (len > 0) {
		   for (len=char_waiting(stdin);  len >= 1; len--)
    		  getchar(stdin);   /* skip bad parity characters */
   	 	   print("*INPUT ERR*\n");
   	    }
		else
			print("*INT*\n");
	}
	breakable(); /* enable breaks. */
	for (;;) {		/* loop forever. */
		do {
			print("\n");
			print("Validation command? ");
			len = readline(buffer);
			if (len == EOF)   /* force a hangup command. */
				exit(0);

		} while (len == 0);

		docommand(buffer);
	}

}


docommand(command)
/* Procedure docommand - process command. */

char	*command;
{

	BUFTYPE	word;
	int		cmdnum, i;
	struct	va_user_record	user_record;
	extern	reset_parse();

	reset_parse();
	if (parse(command,word) == 0)
		return(0);

	cmdnum = binary_wordmatch(word, cmdlist, CMDLIST_LEN);
	
	switch (cmdnum) {

	case -1: print("\"%s\" is not unique.\n",word);
			 break;  /* not unique. */

    case  0: print("\"%s\" is not a command, type 'help' for help.\n",
                word);
			 break;

    case  1: /* create */
		zero(&user_record, sizeof(user_record));  /* zero record */				
			user_record.va_ttyoptions = ECHO + EDIT + ETAB + ERS
				+ EDEL + XON_XOFF + MAPCR;
			user_record.va_backspace = 0x08;
			user_record.va_linedel = 0x1B;
			user_record.va_stateres = TRUE;
			get_date(&user_record.va_lastlogin);
			for (i=1; i<=MODLIST_LEN; i++)
				while(!inp_data(i, &user_record))
					print("Invalid parameter, reenter.\n");
			print("Adding %s to validation file..\n",user_record.va_username);
			interlock_validfile(TRUE);  /* freeze accounting */
			if ((user_record.va_userindex = i = valid_create(&user_record.va_username)) == 32767)
				print("Duplicate entry.\n");
			else
				if (i == 0)
					print("Unable to create entry.\n");
				else 
					if (valid_write(i, &user_record) == 0)
						print("Bad record write (%d).\n",i);
					else
						print("Entry for %s created.\n",user_record.va_username);
			interlock_validfile(FALSE);  /* restore accounting */
			break;			

	case 2:  /* exit */
			print("Returning to control program..\n");		
			transform("/cmds/control", 0, 0);
			break;

	case 4:	 /* help */
			 dohelp(command, "/help/modval");
			 break;

	case 5:  /* list */
			print("The available validation commands are:\n");
			cmd_display(cmdlist, CMDLIST_LEN);
			print("\nThe available modify parameters are:\n");
			cmd_display(modlist, MODLIST_LEN);
			break;

	case 6:  /* modify */
			parse(command, word);
			if ((i = atoi(word)) == 0 && (i = valid_hash(word)) == 0)
				print("No such username.\n");
			else {
				if (read_valid(i, &user_record) == 0) 
					print("Bad valid read.\n");
				else {
					parse(command, word);
					cmdnum = binary_wordmatch(word, modlist, MODLIST_LEN);
					if (cmdnum == 1)
						print("Close and recreate account to change name.\n");
					else
						if (inp_data(cmdnum, &user_record) == TRUE)	
							if (write_valid(i, &user_record) == 0)
								print("Bad valid write.\n");
							else
								print("Entry modified.\n");
    			}
			}
			break;
				
	case 7: /* receive credits.*/
			parse(command,word);
			if ((i = valid_hash(word)) == 0)
				print("No such user.\n");
			else {
 				int		credits;

				parse(command, word);
				if ((credits = atoi(word)) < 1 || credits > 200) {
					if (credits == 0) {
						print("Missing dollar parameter.\n");
						return(-1);
					}
					print("\007 Are you SURE it's %d dollars? ",credits);
					if (getflag() == FALSE)
						return(-1);
				}

					if (abs(credits) >= 4096) {
						print("\007 Credit account overflow!\n");
						return(-1);
					}

					print("Adding %d dollars to account.\n",credits);

/* the following statement gives 8 credits per dollar. */
					credits *= 8;
					send_credits(i, credits);
			}
			break;

	case 8: /* show */
			parse(command,word);
			if ((i = atoi(word)) == 0 && (i = valid_hash(word)) == 0) 		
				print("No such user.\n");
            else 
				if (read_valid(i, &user_record) == 0)
					print("Bad valid read.\n");
				else
					display(&user_record);
			break;

	default:
			print("warn:modval - no such command index!\n");
 			break;
	}
}


cmd_display(list, list_len)
struct	cmdlist_type	list[];
int		list_len;
{
	int	i, skip, index;

	skip = (list_len / 3) + 1;
	for (i=0; i < skip * 3; i++) {
		if (i % 3 == 0)
			print("\n");
		index = (i % 3)*skip + i/3;
		if (index < list_len)
			print("%-11s",list[index].cmd);
	}
	print("\n");
}



display(user_record)
/* procedure display - display user's data record on output. */
struct	va_user_record	*user_record;
{
	char	*data;
	unsigned	opts;
	char	onoff[2][6];
	extern	char	*str_skip();

	strcpy(onoff[FALSE],"*OFF*");
	strcpy(onoff[TRUE],"*ON*");

    print("\n");
	print("%s, Account: %03d.  Userindex: %04d.  Balance: %d cr.\n",
		  user_record->va_username, user_record->va_useracct,
		  user_record->va_userindex, user_record->va_creditbalance);
	print("Last login: %s, %s.\n",
		  dateconv(user_record->va_lastlogin),
		  timeconv(user_record->va_lastlogin));
	print("Bday: %s, CRat: %d.   Account: %s.\n",
		  dateconv(&user_record->va_birthdate), user_record->va_rating,
		  user_record->va_closed ? "CLOSED" : "Open");
	data = user_record->va_stringbuf;
	print(" Name: %s\n",str_skip(0,data));
	print("Addr1: %s\n",str_skip(1,data));
	print("Addr2: %s\n",str_skip(2,data));
	print("Addr3: %s\n",str_skip(3,data));
	print("Phone: %s\n",str_skip(4,data));
	print("Interests: %s\n",str_skip(5,data));
	print("\n");
	opts = user_record->va_ttyoptions;
	print("    Full-duplex......... %s\n",onoff[(opts & ECHO) != 0]);
	print("    BS/space/BS echoed.. %s\n",onoff[(opts & EDEL) != 0]);
	print("    Control-H used...... %s\n",onoff[user_record->va_backspace == 0x08]);
	print("    Mail statements..... %s\n",onoff[!user_record->va_nocharge]);	
	print("    State resident...... %s\n",onoff[user_record->va_stateres]);
	print("    Name anonymous...... %s\n",onoff[user_record->va_anonymous]);
	print("    Line delete char.... %02xH\n",user_record->va_linedel);
}




int inp_data(itemnum, user_record)
/* procedure inp_data - query for an item of data to add to the user_record. 
   returns TRUE if input good, else FALSE */

int	itemnum;
struct	va_user_record	*user_record;
{
	BUFTYPE		buffer;
	int		i;
	extern	char	*str_skip();

	switch(itemnum) {

case -1: 
	print("modify parameter is not unique.\n");
	break;

case 0:
	print("No such parameter.\n");
	break;

case 1:		/* Username */
	print("Enter codename (11 chars max)? ");
	readline(buffer);	
	reset_parse();
	parse(buffer,buffer);
	buffer[11] = '\0';  /* truncate zero */
	if (valid_hash(buffer) != 0)
		print("Duplicate codename.\n");
	else {
		strcpy(user_record->va_username, buffer);
		return(TRUE);
	}
	break;	

case 2:    /* Password */
	print("Enter password (11 chars max)? ");
	readline(buffer);
	reset_parse();
	parse(buffer,buffer);
	buffer[11] = '\0';
	if (buffer[0] != '\0') {
		hashpassword(buffer,user_record->va_hashpw);
		print("PW update complete.\n");
		return(TRUE);
	}
	break;

case 3:		/* Account */
	print("Enter account number (1-255)? ");
	readline(buffer);
	i = atoi(buffer);
	if (i <= 0 || i > 255)
		print("Acct value out of range.\n");
	else {
		if (i == 255)
			print("WARNING! acct set to super-user status.\n");
		user_record->va_useracct = i;
		return(TRUE);
	}
	break;

case 4:		/*  echo flag */
	print("Echo characters (yes)? ");
	user_record->va_ttyoptions &= ~ECHO;
	user_record->va_ttyoptions |= ECHO * getflag();
	return(TRUE);
	break;

case 5:		/*  edel flag */
	print("Send BS,space,BS to delete characters (yes)? ");
	user_record->va_ttyoptions &= ~EDEL;
	user_record->va_ttyoptions |= EDEL * getflag();
	return(TRUE);
	break;

case 6:		/* Control-H */
	print("Control-H instead of DEL key to erase characters (yes)? ");
	user_record->va_backspace = getflag() ? 0x08 : 0x7F;
	return(TRUE);    
	break;           
                     
case 7:		/* credit balance */
	print("Enter credit adjustment? ");
	readline(buffer);
	i = atoi(buffer);
	if (i != 0) {
		print("%d + %d = ",user_record->va_creditbalance, i);
		user_record->va_creditbalance += i;
		print("%d cr.\n",user_record->va_creditbalance);
	}
	return(TRUE);
	break;

case 8:		/* closed */
	print("Is this account open (yes) ? ");
	user_record->va_closed = !getflag();
	return(TRUE);
	break;

case 9:		/* birthdate */
{
	int	m, d;

	print("Birthdate month? (1-12)? ");
	readline(buffer);
	m = atoi(buffer);
	print("Birthdate date? (1-31)? ");
	readline(buffer);
	d = atoi(buffer);
	user_record->va_birthdate = (m << 8) | (d << 1);
	return(TRUE);
	break;
}

case 10:	/* rating */
	print("Credit rating (1-10)? ");
	readline(buffer);
	i = atoi(buffer);
	if (i != 0) {
		user_record->va_rating = i;
		return(TRUE);
	}
	break;

case 11:  /* linedel char */
	print("Enter line-delete code (hex, default 1B)? ");
	readline(buffer);
	i = atoh(buffer);
	if (i != 0)
		user_record->va_linedel = i;
	return(TRUE);
	break;

case 12:  /* nocharge */
	print("Charge credits to user and mail statement (yes)? ");
	user_record->va_nocharge = !getflag();
	return(TRUE);
	break;

case 13: /* name, addr1, addr2, addr3, phone, interests */
{	char        	*string, *strbase;

	display(user_record);
	strbase = &user_record->va_stringbuf; /* 192 char buffer */
	print("Full name? ");
	readline(buffer);
    buffer[79] = '\0';
	strcpy(strbase,buffer);
	print("Address line 1? ");
	readline(buffer);
	buffer[79] = '\0';
	string = str_skip(1,strbase);
	strcpy(string,buffer);
	print("Address line 2? ");
	readline(buffer);
	string = str_skip(2,strbase);
	buffer[min(79,max(0,187 - (int) (string - strbase)))] = '\0';
	strcpy(string, buffer);	                   
	print("Address line 3? ");
	readline(buffer);
	string = str_skip(3,strbase);
	buffer[min(79,max(0,187 - (int) (string - strbase)))] = '\0';
	strcpy(string, buffer);
	print("Phone number? ");	
	readline(buffer);
	string = str_skip(4, strbase);
	buffer[min(79,max(0,187 - (int) (string - strbase)))] = '\0';
	strcpy(string, buffer);
	string = str_skip(5, strbase);
	i = min(79,max(0,187 - (int) (string - strbase)));
 	print("Interests/Hobbies (%d chars max)? ",i);
	readline(buffer);
  	buffer[i] = '\0';
	strcpy(string, buffer); 
	return(TRUE);
}

case 14:  /* resident */
 	print("Does the user live inside the state (yes)? ");
 	user_record->va_stateres = getflag();
 	return(TRUE);
 	break;
 	
case 15: /* anonymous */
	print("Does the user want his real name made public (yes)? ");
	user_record->va_anonymous = !getflag();
	return(TRUE);
	break;

default:
	print("No such option, try again.");
	break;
}

	return(FALSE);
}


int getflag()
/* procedure getflag - read "yes" and "no" from input and return
   proper boolean value */
{
	BUFTYPE		buf;
	extern	reset_parse();

	readline(buf);
	reset_parse();
	parse(buf,buf);
	if (buf[0] != 'n')
		return(TRUE);
	else
		return(FALSE);
}



send_credits(index, numcred)
/* Procedure send_credits - send credit increase to accounting. */
unsigned	index;
int			numcred;
{
	struct	ac_creditmsg		creditmsg;
	struct	ac_creditmsg_reply 	creditmsg_reply;

	creditmsg.ac_checkword = AC_CHECKWORD;
	creditmsg.ac_msgnum = CREDITMSG;
	creditmsg.ac_userindex = index;
	creditmsg.ac_credits = numcred;

	if (send(detach(ACCOUNTING_PORT), &creditmsg, &creditmsg_reply,
		sizeof(creditmsg)) <= 0)
			error("fatal:credit - Unable to send to Acct Admin.\n");

	switch (creditmsg_reply.ac_credit_success) {

case 1: print("Credit update complete.  New balance=%d cr\n",
			creditmsg_reply.ac_creditbalance);
        break;  

case 2: print("\007Credit balance overflow!\n");
		break;

default: /* including zero */
		print("\007Error in accounting credit update.\n");
		break;

	}

}
