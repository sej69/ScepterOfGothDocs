/*			parse - parser.
			84/06/17. A. E. Klietz.

			Copyright (c) 1984.  Alan E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

struct	pa_parmlist {
	char	*pa_pname;
	word	pa_ptype;
};



getword(wrd, numptr)
/* Return next   "string#num" sequence from the input buffer. */
char	*wrd;
word	*numptr;
{
	char	buf[BUFLEN];

	sparse(buffer, wrd);
	*numptr = 0;
	if (parseloc >= 0 && buffer[parseloc-1] == '#') {
		sparse(buffer, buf);
		*numptr = atoi(buf);
	}
	else
		if ((*numptr = atoi(wrd)) != 0)
			/* its a number not a string */
			wrd[0] = '\0';
}
	


word geteditparm(parmlist, worde, data)
/* Return index of given parameter and it's data. */
/*    parameter=data */
struct	pa_parmlist parmlist[];
char	*worde;
word	*data;
{
	char	buf[BUFLEN];
	register	char	ch;
	register	i;

	worde[0] = '\0';
	*data = 0;

	if (parseloc < 0)
		return(0);

	buf[0] = '\0';
	for (i=0; i < BUFLEN-1; ++i) {
		if ((ch = buffer[parseloc++]) == '=')
			break;
		if ((buf[i] = ch) == '\0')
			break;
	}
		
	if (i == 1)
		buf[i++] = ' ';

	buf[i] = '\0';

	string_tolower(buf);
		
	if (ch == '=') {
		for (i=0; parmlist[i].pa_pname != NULL; ++i)
			if (subset(parmlist[i].pa_pname, buf))
				break;

		if (parmlist[i].pa_pname == NULL) {
			mwrite(My_mask, "Unknown parameter - %s.\n", buf);		
			return(0);
		}
	}
	else {
		parseloc = -1;
		mwrite(My_mask, desc(1221));
		/* Missing parameter.\n */
		return(0);
	}

	if (parmlist[i].pa_ptype == DOTHER)
		return(i);

	if (sparse(buffer, worde) <= 0) {
		mwrite(My_mask, desc(1221));
		return(0);
	}

	switch (parmlist[i].pa_ptype) {

case DFLAG:
	if (worde[0] == 't')
		*data = TRUE;
	else
		if (worde[0] == 'f')
			*data = FALSE;
		else {
			mwrite(My_mask, desc(1222), buf);
			/* %s - must be TRUE or FALSE.\n */
			return(0);
		}
	break;

case DNUM:
	if ((*data = atoi(worde)) == 0 && worde[0] != '0') {
		mwrite(My_mask, desc(1223), buf);
		/* %s - must be a number. */
		return(0);
	}
	break;

case DWORD:
	break;
	
	}
	
	return(i);			
}


sparse(buf, worde)
char	*buf, *worde;
{
	register	len;

	len = parse(buf, worde);

	if (len == 4 && (equal(worde, "from") || equal(worde, "with"))
		|| len == 3 && equal(worde, "for")
		|| len == 5 && equal(worde, "spell"))
			return(sparse(buf, worde));

	return(len);
}
