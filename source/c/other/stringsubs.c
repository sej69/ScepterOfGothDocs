/*		stringsubs - string process subroutines.
		83/10/01.  A. E. Klietz.

*/
#include <stdio.h>
#include "/def/aekdef.h"


subset(part, full)
/* Procedure subset - returns TRUE if "part" is a substring
of "full". */

char	*part, *full;
{
	char	ch;

	while ((ch = *part++) == *full++ && ch != '\0')
		;
	return(ch == '\0');	
}


binary_wordmatch(word, wordlist, wordlist_len)
/* Procedure binary_wordmatch scans a alphabetized list for
a unique match and returns the hash code for the word from
the list.  A -1 is returned if not unique, and it issues 
an appropriate error message.  A 0 is returned is not found,
and the calling procedure must issue its own message. */

char	*word;
struct  cmdlist_type  wordlist[];
int		wordlist_len;
{

int		diff, lower, upper, pos;

	if (word[0] == '\0')
		return(0);

	lower = 0;
	upper = wordlist_len - 1;
	do {
		pos = (lower + upper) / 2;
		diff = strcmp(wordlist[pos].cmd, word);       
		if (diff <= 0)
			lower = pos + 1;
		if (diff >= 0)
			upper = pos - 1;
		} while (lower <= upper);

	if (equal(wordlist[pos].cmd,word))
		return(wordlist[pos].cmdnum);

	if (!subset(word, wordlist[pos].cmd)) {
		pos++;
		if (pos > wordlist_len - 1 || !subset(word, wordlist[pos].cmd))
			return(0);
	}
	
	if (pos < wordlist_len - 1)
		if (subset(word, wordlist[pos + 1].cmd))
			return(-1); /* not unique error. */

	return(wordlist[pos].cmdnum);
}
