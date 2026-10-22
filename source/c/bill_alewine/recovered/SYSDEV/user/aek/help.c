/*		help - GamBit help menu.
		83/10/01.  A. E. Klietz.


*/
#include <stdio.h>
#include "aekdef.h"
#define MAXHELPDEPTH 5

dohelp(command,root)
/* Procedure dohelp - process help command.  This procedure pulls
more parameters off the command buffer.  Each parameter is a help
entry in a help-tree.  Each node in the tree contains a list of sub-nodes
to further help entries.  Format for a help file is:


parameter1,   /help/file_parm1
parameter2,   /help/file_parm2
moreinfo,     /help/stuff
<blank line>

Short description....


*/

char	*command, *root;
{
    int		return_status = -2;
	BUFTYPE depthlist[MAXHELPDEPTH]; /* list of nested help topic names. */
	FILE	*helpfp;

while(return_status == -2)   /* "help" entered again. */
	return_status = nexthelp(command, root, depthlist, 0);

	print("You are leaving the help directory.\n");
}


nexthelp(command, filename, depthlist, depth)
    
char	*command, *filename;
BUFTYPE	*depthlist;
int		depth;  /* recursion depth. */
{
	BUFTYPE		newname, buffer;
	boolean		firstime = TRUE;
	int			len, temp, return_status;
	extern	int	parseloc;  /* next position in parse routines for search. */
	FILE		*helpfp;


if (depth >= MAXHELPDEPTH) { /* nested too deep. */
	print("help:warn - help recursion nested too deep.\n");
	return(-1);
}

if ((helpfp = fopen(filename,"rv")) == 0) 
	return(0);
	
return_status = 0;

while(return_status == 0)	
	if (parse(command,newname) == 0) { /* bottom of recursion. */
		if (firstime) { /* display entire help file. */
			rewind(helpfp);
			do {
				len = freadline(helpfp, buffer);
			} while (len > 0);  /* skip arg list. */
		
			print("\n");
			while (freadline(helpfp,buffer) != EOF) /* display help entry. */
				print("%s\n",buffer);
		}

		rewind(helpfp);
		freadline(helpfp, buffer);
		temp = parseloc;
		reset_parse();
		if (parse(buffer, buffer) > 0) { /* if args, then prompt. */
/* KLUDGE: If !brief || firstime then... */
/* pass 'buffer' as access to free string space. buffer destroyed */
				print_help_parms(helpfp, depth, buffer);
			firstime = FALSE;
			print_help_prompt(depthlist, depth);
			reset_parse();
	     	if ((len = readline(command)) <= 0) { /* go up to previous level. */
				fclose(helpfp);
				return(0);
			}
		}			
		else {
			parseloc = temp;
			firstime = FALSE;
			fclose(helpfp);
			return(0);
		}
	}
	else { /* descend to next recursion level. */
		firstime = FALSE;
		rewind(helpfp);
		temp = parseloc;
		do { /* scan for match to 'newname' */
			freadline(helpfp, buffer); /* buffer used as temp */
			reset_parse();
			len = upper_parse(buffer, buffer); /* get topic */
			strcpy(&depthlist[depth], buffer);
			string_tolower(buffer); /* buffer used as temp variable. */
			if (subset(newname, buffer))
				break;
         } while (len > 0);
		parse(buffer, buffer); /* now read in file name. */
		parseloc = temp;
		if (len <= 0)  { /* no match found. */
			if (!subset(newname,"back")) 
				if (subset(newname,"quit")) {
					fclose(helpfp);
					return(-1); /* abort */
				}
				else 
					if (subset(newname,"help")) {
						fclose(helpfp);
						return(-2); /* flag dohelp to loop again. */
					}
					else {
						print("\"%s\" is not one of the available options.\n",newname);
/* KLUDGE: if brief then do...     print_help_parms(helpfp, depth); */
						reset_parse();
						command[0] = '\0'; /* trash remainder of command line. */
					}
			else {
        	    fclose(helpfp);
				return(0); /* "back" */
			}
		}
		else { /* open new file. */
			print("\n");
		  	for (len=1; len <= depth; len++)
				print("   ");         
			print("%s\n",&depthlist[depth]);
			fclose(helpfp);
			return_status = nexthelp(command, buffer, depthlist, depth + 1);
			if (return_status < 0)
				return(return_status); /* pass the buck */
			if ((helpfp = fopen(filename,"rv")) == 0)
				return(return_status);

		}
	}
	fclose(helpfp);
	return(return_status);
}


print_help_parms(helpfp, depth, buffer)
/* Procedure print_help_parms - display the topic parameters in the given
help file.  */

FILE	*helpfp;
int		depth;
char	*buffer;
{
	int			num,len,temp;
	extern	int	parseloc;  /* location of parser pointer (iosubs) */

	rewind(helpfp);
	temp = parseloc;
	num = 0;
	print("\n");
	if (depth == 0)
		print("Please choose one of the following topics:\n");
	else
		print("Choose one of the following subtopics:\n");

	do { /* display all parameters */
		freadline(helpfp, buffer);
		reset_parse();
		len = upper_parse(buffer, buffer);
		if (len > 0) { /* display param */
			if (num % 5 == 0)
				print("\n");
			num++;
			print(" %-11s",buffer);
		}
	} while (len > 0);
    
	if (num % 5 > 3 && depth > 0 || num % 5 > 4) /* go to next line. */
		print("\n");
	if (depth > 0) 
		print(" BACK       ");
	print(" QUIT\n");
	parseloc = temp;
}                                    


print_help_prompt(depthlist, depth)
/* Procedure print_help_prompt - display the help prompt, listing all
previous entries on the line.  The previous entries are stored in the
string array "depthlist"  */

BUFTYPE *depthlist;
int		depth;
{
	int	i;

	print("\n");
	if (depth == 0) 
		print("Select topic? ");
	else {
		print("Select ");
		for(i=0; i<depth; i++)
			print("%s ",&depthlist[i]);
        print("subtopic? ");
	}
}
