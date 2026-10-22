/*		menu - GamBit menu program.
		83/10/01.  A. E. Klietz.

		'menu' is a program that handles all introductory communication
with the user.

		If argc > 1 then skip the sysnote and other login checks.

*/
#include <stdio.h>
#include <dev.h>
#include "aekdef.h"
#include "globdef.h"
#include "validef.h"
#include "appdef.h"

#define BEQUEATH_TTY 0x04
#define EXC_BREAK 2
#define IDLE   0x0004

extern unsigned My_tid, Dads_tid;
extern char My_tty, User_num;
extern int  Error_status;

struct	gl_user_datum		*user_data_ptr;
struct	gl_system_datum		*system_data_ptr;


#asm "seg 2" /* force into constant data segment */
static char  cmdnames[][12] = {

	"bye        ",
	"clock      ",
	"control    ",
	"credit     ",
	"diplomacy  ",
	"goodbye    ",
	"help       ",
	"list       ",
	"mail       ",
	"news       ",
	"password   ",
	"scepter    ",
	"send       ",
	"set_option ",
	"talk       ",
	"tasks      ",
	"users      ",
	"who        "
};


#asm "seg 2"   /* force into constant data segment */
static struct cmdlist_type cmdlist[] = {
	0, 1,
	0, 15,
	0, 13,
	0, 12,
	0, 2,
	0, 1,
	0, 3,
	0, 11,
	0, 4,
	0, 5,
	0, 16,
	0, 6,
	0, 17,
	0, 14,
	0, 7,
	0, 8,
	0, 9,
	0, 10
};


#define CMDLIST_LEN (sizeof(cmdlist) / sizeof(struct cmdlist_type))



main(argc, argv)
int		argc;
char	*argv[];
{
BUFTYPE buffer;
int		len;
FILE	*fp;
extern	break_handler();
boolean	first_cmd = TRUE;

	
	unbreakable(); /* disable breaks here. */
	set_glob();    /* set global pointers */
	
	if (argc <= 1) { /* if initial call to menu. */
		if (Dads_tid == IDLE) /* dont change console's app */
			@&user_data_ptr[My_tty].gl_apnum = AP_MENU;
		print("                _\n");
		print("Welcome to the GamBit Menu.\n");
		if ((fp = fopen("/sys/note","r")) > 0) { /* show sysnote. */
			while (freadline(fp,buffer) != EOF)
				print("%s\n",buffer);
    	    fclose(fp);
		}
		
		if (!(@&system_data_ptr->gl_sysopen))
			if ((unsigned) User_num != 255) {
				print("System closed, sorry.\n");
				exit(0);
			}
			else
				print("(System closed for non-staff.)\n");

		print(" \n");
		print("Type 'help' for help.\n");
	}	
	else
		if (Dads_tid == IDLE) /* console does not issue sendap's */
			sendap(AP_MENU);


	for (len=0; len < CMDLIST_LEN; len++) /* set pointers in cmdlist */
		cmdlist[len].cmd = cmdnames[len];

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

	if (first_cmd) { /* list commands only on first iteration of cmd loop */
		first_cmd = FALSE;
		print("\n");
		list_cmds();
		print("\nType 'list' to list these commands.\n");
	}
		
	for (;;) {		/* loop forever. */
		do {
			print("\n");
			print("Enter menu command? ");
			len = readline(buffer);
			if (len == EOF) {  /* force a hangup command. */
				strcpy(buffer,"bye");
				len = 3;
			}
		} while (len == 0);

		if ((User_num == -1) && equal(buffer, "manager")) {
			if (Dads_tid == IDLE) 
				sendap(AP_SYSTEM); /* switch tables + journal entry */
			transform("/cmds/qnxsh", 0, 0);
		}

		docommand(buffer);
	}

}


docommand(command)
/* Procedure docommand - process menu command. */

char	*command;
{

	BUFTYPE	word;
	int		cmdnum, i;
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

	case  1: /* bye */
		{	int credit_left;

		     credit_left = credit_update(My_tty, -1); /* round result */
			 print("Goodbye, come again!\n");
			 print("Logoff at %s, on %s.\n",time(),date());
			 print("%d credits used.  Account balance= %d cr.\n",
				@&user_data_ptr[My_tty].gl_startcredits - credit_left,
				credit_left);
			 exit(0);
		}
			 break;

	case  2:
	case  4:
	case  6:
			 print("Sorry, that feature hasn't been implemented yet.\n");
			 break;

	case  3: /* help */
			 dohelp(command, "/help/help");
			 break;

	case  5: /* news */
			 {
			 	FILE	*fp;
				if ((fp = fopen("/sys/news","r")) > 0) { 
					BUFTYPE	buffer;

					while(freadline(fp,buffer) != 0)
						print("%s\n",buffer);
					fclose(fp);
				}
				else
					print("There is no news today.\n");
			 };
			 break;

	case 7:  /* talk */
			sendap(AP_TALK);
			transform("/sys/io", "21", "gtalk");
			sendap(AP_MENU);
			break;

	case  8: /* task */
			{
				BUFTYPE		word2;
                        
				if (parse(command,word2) > 0)
					if (subset(word2,"size"))
						call("/cmds/task","+s",0);
					else
						print("Unknown task option.\n");
				else
					call("/cmds/task", 0, 0);
			};
			break;

	case 10: /* who */
			if (parse(command,word) <= 0)
				call("/cmds/whog",0,0);
			else {
				int i;
				unsigned	index = 0;
				BUFTYPE		name;
				
				if (subset(word, "gambit")) {
					print(" _        _\n");
					print("GamBit - GamBit Multi Systems, Inc.\n");
					return(0);
				}

				for (i=0; i<NUMDEVICES && index == 0; i++) {
					strcpy_star_from_at(name, user_data_ptr[i].gl_username);				
					string_tolower(name);
					if (subset(name, word))								
						index = @&user_data_ptr[i].gl_userindex;
				}
				if (index == 0 && (index = valid_hash(word)) == 0)
						print("Name not found.\n");
				else
					user_display(index);
			}
			break;

	case 11: /* list commands */
			list_cmds();
			break;
	
	case 12: /* credits left */
			{
				int	credit_left;

				credit_left = credit_update(My_tty, 0);
				print("You have used %d credits since login,\n",
					@&user_data_ptr[My_tty].gl_startcredits - credit_left);
				print("and have %d credits remaining in\n",credit_left);
				print("your account.\n");
			}
			break;


	case 13: /* control system */
			if (User_num != -1)
				print("Command restricted, sorry.\n");
			else
				transform("/cmds/control", 0, 0);
			break;

	case 14: /* set command */
{
	struct	gl_user_datum	*user_ptr;

			user_ptr = &user_data_ptr[My_tty];

			if (User_num == 1) { /* guest account */
				print("Sorry, the guest account cannot change its terminal parameters.\n");
				return(-1);
			}

			while (parse(command,word) <= 0) {
				print("Select one of these options:\n");
				print("     backspace\n");
				print("     duplex\n");
				print("     echo_backspace\n");				
				print("     line_erase\n");				
				print("\nEnter set option? ");
	        	reset_parse();
				if (readline(command) <= 0)
					return(0);
			}
			if (subset(word, "backspace")) {
				print("Do you want to use\n");
				print("     1) backspace key (Control-H)\n");
				print("     2) delete key    (DEL)\n");
				print("for erasing a character?\n");
				print("\nSelect 1 or 2 ? ");
				readline(command);
				if ((i = atoi(command)) < 1 || i > 2) {
					print("Incorrect value.\n");
					return(-1);
				}
				else
					@&user_ptr->gl_backspace =
						(i == 1) ? 0x08 : 0x7F;			
			} else if (subset(word, "duplex")) {			
				print("Do you want to set\n");
				print("     1) full duplex\n");
				print("     2) half duplex\n");
				print("\nSelect 1 or 2 ? ");
				readline(command);
				if ((i = atoi(command)) < 1 || i > 2)  {
					print("Incorrect value.\n");
					return(-1);
				}
				else {
					@&user_ptr->gl_ttyoptions &= ~ECHO;
					@&user_ptr->gl_ttyoptions |= (i == 1 ? ECHO : 0);
				}
			} else if (subset(word, "echo_backspace")) {
				print("Do you want\n");
				print("     1) Backspaces echoed with space/back/space\n");
				print("     2) No special echoing of backspaces\n");
				print("\nSelect 1 or 2 ? ");
				readline(command);
				if ((i = atoi(command)) < 1 || i > 2) {
					print("Incorrect value.\n");
					return(-1);
				}
				else {
					@&user_ptr->gl_ttyoptions &= ~EDEL;
					@&user_ptr->gl_ttyoptions |= (i == 1 ? EDEL : 0);
				}
			} else if (subset(word, "line_erase")) {
				print("Select the character you wish to use for erasing a line:\n");
				print("     1) Control-U  (NAK)\n");
				print("     2) Control-X  (CAN)\n");
				print("     3) Escape key (ESC)\n");
				print("\nSelect 1, 2, or 3 ? ");
				readline(command);
				if ((i = atoi(command)) < 1 || i > 3) {
					print("Incorrect value.\n");
					return(-1);
				}
				else {
					switch (i) {
		case 1:	i = 0x15;
				break;
		case 2: i = 0x18;
				break;
		case 3: i = 0x1B;
				break;
		}
					@&user_ptr->gl_linedel = i;
				}
			} else {
				print("No such option.\n");
				return(-1);
			}
	        set_glob();  /* set up new parameters */
			print("Option set to new value.\n");
			break;
}
				
	case 15:  /* clock */
			print("%s, %s.  %s time.\n", time(), date(),
				(@&system_data_ptr->gl_peakflag ? "Peak" : "Non-peak"));
			{	unsigned	bindate[2];

				bindate[0] = @&system_data_ptr->gl_binaryuptime[0];
				bindate[1] = @&system_data_ptr->gl_binaryuptime[1];
				print("System up at %s, %s.\n", timeconv(bindate),
					dateconv(bindate));
			}
			break;

	case 16: /* change_pw */
			if (User_num == 1) {
				print("Sorry, the guest account cannot change its password.\n");
				return(-1);
			}

			print("Enter old password? ");
			readline(word);
			reset_parse();
			parse(word,word);
			hashpassword(word, word);
			{
				struct	va_user_record	user_record;
				
				if (read_valid(@&user_data_ptr[My_tty].gl_userindex,
					&user_record) == 0) 
					print("Bad validuz read.\n");
				else 
					if (!equal(user_record.va_hashpw, word))
						print("Improper password.\n");
					else {
						BUFTYPE	word2;

						print("Enter new password? ");
						readline(word);
						print("Enter new password again? ");
						readline(word2);
                        if (!equal(word, word2))
							print("Passwords didn't match.\n");
						else {
							reset_parse();
							parse(word, word);
							word[11] = '\0';
							if (strlen(word) < 4)
								print("Passwords must be at least 4 characters long.\n");
							else {
								hashpassword(word, user_record.va_hashpw);
    							if (write_valid(user_record.va_userindex,
										&user_record) == 0)					
									print("Bad validuz write.\n");					
								else								
									print("Password changed.\n");									
							}
						}
					}
			}
            break;

	case 9: /* users */
			print("\nList of all users:\n");
			{
				FILE *fp;
				int  i = 0;
				char	name[16];

				fp = fopen("/val/valindx", "rv");
				
				fiseek(fp, 256);  /* skip hash table */
				while(fget(name, 16, fp) > 0) {
					print("%-11s ",name);
					if (++i % 3 == 0)
						print("\n");
				}
				print("\n");
				fclose(fp);
			}
			break;


	case 17: /* send msg to user */
	{
			extern  int		parseloc;
			int		len, ttynum = -1;
			FILE	*fp;
			char	name[12], tty_name[8];

			
			while (parse(command, word) <= 0) {
				print("Send message to whom? ");
				readline(command);	
				reset_parse();
			}

			len = (parseloc <= 0 ? 0 : 1);
			while (len == 0) { /* end of line */
				print("Enter message to send? ");
				len = readline(command);
				reset_parse();
			}
			while (isseparator(command[parseloc]))
				parseloc++;  /* skip leading spaces */

			for (i=0; i<NUMDEVICES && ttynum == -1; i++) {
				strcpy_star_from_at(name, user_data_ptr[i].gl_username);				
				string_tolower(name);
				if (subset(word, name))								
					ttynum = i;
			}

			if (ttynum < 0) {
				print("No such user logged in.\n");	
				return(-1);
		 	}

			strcpy_star_from_at(tty_name, user_data_ptr[ttynum].gl_ttyname);
			fp = fopen(tty_name, "wv");
			strcpy_star_from_at(word, user_data_ptr[My_tty].gl_username);
			tfprintf(fp, "\n\007Message from %s:  \"%s\"\n", word,
					&command[parseloc]);
			fclose(fp);
			print("Message sent to %s.\n", name);
			break;
	}


	default:
			print("warn:menu - no such command index!\n");
			break;
	}
}

list_cmds()
/* list available commands */
{
		int		i, skip, index;

		print("The available menu commands are:\n");
		{
			skip = (CMDLIST_LEN / 3) + 1;
			for (i=0; i < skip * 3; i++) {
				if (i % 3 == 0)
					print("\n");
				index = (i % 3)*skip + i/3;
				if (index < CMDLIST_LEN)
					print("%-11s",cmdlist[index].cmd);
			}
		}
		print("\n");
}




user_display(index)
/* procedure user_display - display some information about user */
unsigned	index;
{
	struct	va_user_record	user_record;
	BUFTYPE	word;
	extern	char	*str_skip();
	int		i;

	if (read_valid(index, &user_record) != 0) {

		user_record.va_username[0] = toupper(user_record.va_username[0]);
		user_record.va_stringbuf[0] = toupper(user_record.va_stringbuf[0]);
		
		print("%s's real name is %s.  %s's interests are\n",
			user_record.va_username, (user_record.va_anonymous ?
			"Anonymous" : user_record.va_stringbuf),
			user_record.va_username);
		print("%s\n", str_skip(5, user_record.va_stringbuf));
		reset_parse();
		strcpy(word, str_skip(3, user_record.va_stringbuf));
		if (word[0] == '\0') {
			reset_parse();
			strcpy(word, str_skip(2, user_record.va_stringbuf));
		}
			for (i=0; word[i] != '\0'; i++)			
				if (atoi(&word[i]) != 0) /* dont copy zip code */
					word[i--] = '\0';

			print("%s lives in %s.\n", user_record.va_username, 
				word);
	}
}
