/*		desc - string file descriptions.
		84/06/08.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"

static	unsigned	edescrp = 0;
static		char	*textline;

static	char	text[256];

char	*desc(index)
word	index;
{
	if (edescrp == 0)
		if ((edescrp = random_open("/data/scepter/edescrp")) == 0)
			crash("Unable to open edescrp file.\n");

	if (random_read(edescrp, index, text, 256) == 0)
		text[0] = '\0';

	text[255] = '\0';
	return(text);
}



writedesc(index, line)
word	index;
char	*line;
{
	register i, j;

	j = strlen(line) + 1;
	for (i=0; i < j-1; ++i)
		if (line[i] == '\\' && line[i+1] == 'n') {
			line[i] = '\n';
			strcpy(&line[i+1], &line[i+2]);
			--j;
		}

	if (j <= 1)
		j = 0;
	
	if (random_write(edescrp, index, line, j) != j)
		crash("Unable to write line %d into edescrp file!\n", index);
}



char  *printdesc(line, phrase, brief)
word	line, phrase;
boolean	brief;
{
	char	*bufptr;
	register	char *buf;			
	register	i, j;
	register	len, phrasenum;
	boolean	brmark;

	if (edescrp == 0)
		if ((edescrp = random_open("/data/scepter/edescrp")) == 0)
			crash("Unable to open edescrp file.\n");

	if ((len = random_allocread(edescrp, line, &bufptr, 256)) == 0) {
		tsprintf(text, "Bad description line (#%d)", line);
		return(text);
	}

	buf = bufptr;
	
	if (phrase < 1) { /* Choose a random phrase */
		for (i=0, phrase = 0; i < len; )
			if (buf[i++] == '/') 
				++phrase;
		phrase = ran(phrase - 1);
	}

	for (i=0, phrasenum = 0; i < len; )
		if (buf[i++] == '/') {
			++phrasenum;
			if (phrasenum == phrase)
				break;
		}

	if (i >= len) {
		tsprintf(text, "Bad phrase (#%d) for line #%d", phrase, line);
		free(buf);
		return(text);
	}

	if (textline != NULL)
		free(textline);

	textline = alloc(len);

	brmark = FALSE;
	for (j=0; i < len; ++i) {
		if (buf[i] == '/')
			break;
		
		if (buf[i] == '#') 
			brmark = !brmark;
		else {
			if (!brmark || !brief) {
				if (buf[i] == '\\' && buf[i+1] == 'n') {
					++i;
					textline[j++] = '\n';
				}
				else
					textline[j++] = buf[i];
			}
		}
	}

	textline[j] = '\0';
	free(buf);
	return(textline);
}
