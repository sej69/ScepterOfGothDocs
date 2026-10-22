/* 		iosubs - Standard input/output subroutines.
		83/10/01.	A. E. Klietz.

		These are the input/output routines used by most
GamBit programs.	
*/

#include <stdio.h>
#include "aekdef.h"
#include "globdef.h"

extern  byte  My_tty;
static	int	  *monflag,  montty;   /* addr of global mon flag, and local copy. */
static	FILE  *monfp;   /* file pointer of monitoring tty */

int		parseloc; /* location of next character in buffer for parsing. */



clean_string(string)
/* Procedure clean_string - remove parity bit (high bit) and skip control
characters (other than tabs) */
char	*string;
{
	register int i;
	register char ch;
	int		j, limit;

	i=0;
	while (string[i] &= 0x007F)   /* strip parity */
		i++;
	limit = i - 1; 
	i=j=0;
    for (i=0; i <= limit; i++)
		if ((ch = string[i]) >= ' ' && ch != 0x7F || ch == '\t')
			string[j++] = ch;
	string[j] = '\0'; /* ensure termination of string */
}



freadline(fp,string)
/* Procedure freadline - read a line of text from a file.
returns length of inputted line, -1 if end-of-file. */

FILE *fp;
char *string;
{
int		len;

	len = fgetstr(fp,string,BUFLEN);
	if (len > 0)
		clean_string(string);
	return(len);
}


readline(string)
/* Procedure readline - read a line of text from input. */
char *string;

{
	int		len;
	unsigned	bindate[2];
	extern	struct	gl_user_datum	*user_data_ptr;

	if (user_data_ptr == 0)
		error("fatal:readline - undefined user_data_ptr!\n");

	len = freadline(stdin, string);
	if (montty != 0) /* being monitored */
		tfprintf(monfp, "*%d> %s\n", My_tty, string);
	
	get_date(bindate);  /* update last cr for timeout check */
	@&user_data_ptr[My_tty].gl_binarylastcr[0] = bindate[0];
	@&user_data_ptr[My_tty].gl_binarylastcr[1] = bindate[1];

	return(len);
}




print(string, a1, a2, a3, a4, a5, a6)
/* Procedure print - send a line of text to output using tfprintf */
char	*string;
int		a1, a2, a3, a4, a5, a6;

{
extern  struct gl_user_datum		*user_data_ptr;
int		ttynum;
char	port_name[8];

	if (monflag == 0) { /* not initialized */
		if (user_data_ptr == 0)
			error("fatal:print - user_data_ptr undefined.\n");
		monflag = &user_data_ptr[My_tty].gl_monitor;
    }

	tfprintf(stdout, string, a1, a2, a3, a4, a5, a6);

	if (montty != @monflag) { /* change in monitor status */
		montty = @monflag;
		if (montty == 0) /* end of monitoring */
			fclose(monfp);
		else { /* beginning of monitoring */
			strcpy_star_from_at(port_name, user_data_ptr[
				(montty == -1 ? 0 : montty)].gl_ttyname);
			monfp = fopen(port_name, "wv");
		}
	}
		
	if (montty != 0) {  /* being monitored! */
		tfprintf(monfp, "%d> ",My_tty);
		tfprintf(monfp, string, a1, a2, a3, a4, a5, a6);
	}
}


isseparator(ch)
/* Procedure isseparator  - return true if ch is one of the
characters " ,.?\n\t#" */

int	ch;
{
	char	*delim = " ,.?\n\t#";
	char    ch2;
	int		pos    = 0;

	while ((ch2 = delim[pos++]) != '\0')
		if (ch == ch2)
			return(1);
	
	return(0);
}


string_tolower(string)
/* Procedure string_tolower - convert a string to lowercase. */

char	*string;
{
	int		pos	= 0;

	while (string[pos] != '\0') {
		string[pos] = tolower(string[pos]);
        pos++;
	}
}


reset_parse()
/* Procedure reset_parse - reset the parse counter. */

{
	parseloc = 0;
}


upper_parse(buffer, word)
/* Procedure parse - extract a single word from 'buffer' to 'word'.
The size of 'word' must equal the size of 'buffer' to avoid over-
flowing the word string.    The procedure returns the length of the
word, zero if end-of-string. Words are not* converted to lowercase. */

char	*buffer, *word;
{
int	wordpos;  /* character pointer for 'word'.*/
char ch;


	if (parseloc == -1) { /* end of buffer already encountered. */
		word[0] = '\0'; /* guarantee end of string. */
		return(0);
	}

	while ((ch = buffer[parseloc++]) != '\0')
		if (!isseparator(ch)) /* skip separators. */
			break;

	if (ch == '\0') { /* return zero if no non-seps found. */
		parseloc = -1;
        word[0] = '\0'; /* guarantee end of string. */
		return(0);
	}

	wordpos = 0;
	do {  /* fill word */
		word[wordpos++] = ch;
	    ch = buffer[parseloc++];
	} while ((ch != '\0') && !isseparator(ch) && wordpos < BUFLEN - 1);

	word[wordpos] = '\0'; /* add trailing zero to make proper string. */

	if (ch == '\0') /* end of buffer? */
		parseloc = -1;
	else {
		while ((ch = buffer[parseloc++]) != '\0')
			if (!isseparator(ch)) /* skip separators. */
				break;
		parseloc--;
		if (ch == '\0')
			parseloc = -1;
	}

	return(wordpos);
}
	
parse(string, word)
/* Parse - parse string into lowercase words.  
This procedure is identical to upper_parse, except it converts words
to lowercase. */

char	*string, *word;
{
	int		len;

	len = upper_parse(string, word);
	string_tolower(word);  /* convert to lowercase. */

	if (len <= 3) /* mask out short prepositions and articles. */
		if (	equal(word,"is") ||
				equal(word,"the") ||
				equal(word,"to") ||
				equal(word,"in") ||
				equal(word,"at")
		)
			return(parse(string, word));  /* recursively get the next word. */

	return(len);
}
