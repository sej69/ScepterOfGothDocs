/*			find - find things.
			84/06/12.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"

#define MAXNAMES 30 /* maximum number of objects in a room for parsing */


boolean	lsubset(prt, fll)
/* Return TRUE if "part" is a lowercase substring of "full" */
char	*prt, *fll;
{
	register	char	*part, *full;
	register	char	ch;

	part = prt;
	full = fll;
	while ((ch = *part++) == tolower(*full++) && ch != '\0')
		;

	return(ch == '\0');
}



word wordmatch(wrd, count, namelist)
/* Return index of wrd in array.  -2 if not unique, and it
issues its own message. -1 if not found and the calling
program must issue its own message. */
char	*wrd;
word	count;
char	*namelist[];
{
	register	i,j;

	if (count < 0 || count > 255)
		crash(desc(900));
		/* Sce900 - Bad parse count!\n */

	if (wrd[0] == '\0')
		return(-1); /* empty string does not match anything */

	for (i=0; i < count; ++i)
		if (equal(wrd, namelist[i]))
			break;

	if (i < count)
		return(i); /* direct match */

	for (i=0; i < count; ++i)
		if (lsubset(wrd, namelist[i]))
			break;

	if (i >= count)
		return(-1);

	for (j=i+1; j < count; ++j)
		if (lsubset(wrd, namelist[j])) {
			char	buf[BUFLEN], buf2[BUFLEN];
			word	temp;

			temp = parseloc;
			reset_parse();
			parse(namelist[i], buf);
			if (equal(wrd, buf)) {
				parseloc = temp;
				return(i);
			}
			reset_parse();
			parse(namelist[j], buf2);
			if (equal(wrd, buf2)) {
				parseloc = temp;
				return(j);
			}
			parseloc = temp;
			if (!equal(buf, buf2))
				break;
		}

	if (j < count) { /* if not unique */

		mwrite(My_mask, "\"%s\" is not unique.\n", wrd);
		return(-2);
	}
	else
		return(i);
}
		


struct	mo_monster	*findmonster(wrd, nm, monstertail)
char	*wrd;
word	nm;                         
struct	mo_monster	*monstertail;
{
	register	struct	mo_monster	*monster;
	char		*namelist[MAXNAMES];
	register	count, index;
	extern		word	parseloc;

	for (count=0, monster = monstertail; monster != NULL && count < MAXNAMES;
			++count, monster = monster->mo_next)
		namelist[count] = monster->mo_name;

	if (count >= MAXNAMES)
		mwrite(My_mask, desc(902));
		/* Sce902 - Monster table overflow.\n */

	if ((index = wordmatch(wrd, count, namelist)) < 0)
		return(NULL);

	for (monster = monstertail, count = 0; count < index;  ++count)
			monster = monster->mo_next;
	
	for (; monster != NULL; monster = monster->mo_next) 
		if ((nm == monster->mo_num || nm == 0) && 
				lsubset(wrd, monster->mo_name))
			break;

	return(monster);
}


struct	ob_object	*findobject(wrd, nm, objecttail)
char	*wrd;
word	nm;
struct	ob_object	*objecttail;
{
	register	struct	ob_object	*object;
	register	i;
	word		nummatch, count, index;
	char		*namelist[MAXNAMES];

	
	for (count=0, object = objecttail; object != NULL && count < MAXNAMES;
			++count, object = object->ob_next)
		namelist[count] = object->ob_name;

	if (count >= MAXNAMES)
		mwrite(My_mask, desc(903));
    	/* Sce903 - Object table overflow. */

	if ((index = wordmatch(wrd, count, namelist)) < 0)
		return(NULL);

	for (object = objecttail, i = 0; i < index; ++i)
		object = object->ob_next;

	if (nm <= 1)
		return(object);
	
	nummatch = 1;
	for (i = index + 1; nummatch < nm && i < count; ++i) {
		object = object->ob_next;
		if (lsubset(wrd, namelist[i]))
			++nummatch;
	}

	if (nummatch < nm)
		return(NULL);

	return(object);
}


struct	us_user	*findplayer(wrd, playertail)
char	*wrd;
struct	us_user	*playertail;
{
	register	struct	us_user	*player;
	register	count, i, index;
	char		*namelist[MAXNAMES];

	
	for (count=0, player = playertail; player != NULL && count < MAXNAMES;
			++count, player = player->us_next)
		namelist[count] = player->us_name;

	if (count >= MAXNAMES)
		mwrite(My_mask, desc(904));
    	/* Sce904 - player table overflow. */

	if ((index = wordmatch(wrd, count, namelist)) < 0)
		return(NULL);

	for (player = playertail, i = 0; i < index; ++i)
		player = player->us_next;

	if (player != NULL && player->us_nonexistant && !user->us_ssj)
		return(NULL);

	return(player);
}



struct	us_user	*finduser(wrd, usrtail)
char	*wrd;
struct	us_user	*usrtail;
{
	register	struct	us_user	*usr;
	register	count, i, index;
	char		*namelist[MAXNAMES];

	
	for (count=0, usr = usrtail; usr != NULL && count < MAXNAMES;
			++count, usr = usr->us_nextuser)
		namelist[count] = usr->us_name;

	if (count >= MAXNAMES)
		mwrite(My_mask, desc(905));
    	/* Sce905 - user table overflow. */

	if ((index = wordmatch(wrd, count, namelist)) < 0)
		return(NULL);

	for (usr = usrtail, i = 0; i < index; ++i)
		usr = usr->us_nextuser;

	if (usr != NULL && usr->us_nonexistant && !user->us_ssj)
		return(NULL);

	return(usr);
}

                              
char	*printobj(obj, singular)
/* Display the name of an object.  If singular = TRUE then
display "the", else the article (a, an) */
struct	ob_object	*obj;
boolean	singular;
{
	register	loc;
	char		buf2[BUFLEN], buf3[BUFLEN];
	static		char	buf[160];
	register	char		*txt;
	register	struct	ob_object	*object;
	word		temparse;
	extern		word	parseloc;

	object = obj;
	loc = 0;
	if (singular && (object->ob_article != NONE)) {
		strncpy(&buf[loc], "the ", 160 - loc);
		loc += 4;
	}
	if (!singular) 
		if (object->ob_obclass != DOOR) {
			switch	(object->ob_article) {
				case A:	txt = "a ";
				break;

				case AN: txt = "an ";
				break;
	
				case THE: txt = "the ";
				break;

				case SOME: txt = "some ";
				break;

				default:
				case NONE: txt = "";
				break;
			}
			strncpy(&buf[loc], txt, 160 - loc);
			loc += strlen(txt);
		}
		else {
			switch (object->ob_article) {
				case A:
				case AN:
					if (object->ob.ob_door.ob_dclosed)
						txt = "a ";
					else
						txt = "an ";
				break;

				case THE: txt = "the ";
				break;

				case SOME: txt = "some ";
				break;

				default:
				case NONE: txt = "";
				break;
			}
			strncpy(&buf[loc], txt, 160 - loc);
			loc += strlen(txt);
			if (object->ob.ob_door.ob_dclosed)
				if (object->ob.ob_door.ob_dlocked > 0)
					txt = "locked ";
				else
					txt = "closed ";
			else
				txt = "open ";
			strncpy(&buf[loc], txt, 160 - loc);
			loc += strlen(txt);
		}

	if (object->ob_obclass == COINS) {
		tsprintf(buf2, "%u ", object->ob_price / object->ob.ob_coins.ob_multiplier);
		strncpy(&buf[loc], buf2, 160 - loc);
		loc += strlen(buf2);
	}

	if (object->ob_magic) {
		strncpy(&buf[loc], "magic ", 160 - loc);
		loc += 6;
	}

	temparse = parseloc;		
	reset_parse();
	upper_parse(object->ob_name, buf2);
	if (parseloc > 0) {
		txt = object->ob_name + parseloc;
		tsprintf(buf3, txt, buf2);		
		txt = buf3;
	}
	else
		txt = buf2;
	parseloc = temparse;
	strncpy(&buf[loc], txt, 160 - loc);
	loc += strlen(txt);

	if (!singular) {
		if (object->ob_invisible) {
			strncpy(&buf[loc], " (inv)", 160 - loc);
			loc += 6;
		}

		if (object->ob_obclass == WEAP && object->ob_magic) {
			tsprintf(buf2, " (+%d)", abs(object->ob.ob_weap.ob_weaplus));
			strncpy(&buf[loc], buf2, 160 - loc);
			loc += strlen(buf2);
		}

		if (object->ob_obclass == SHIELD || object->ob_obclass == ARMOR) {
			tsprintf(buf2, " (%d)", abs(object->ob.ob_shield.ob_shplus));
			strncpy(&buf[loc], buf2, 160 - loc);
			loc += strlen(buf2);
		}

		if (object->ob_obclass == TREASURE) {
			tsprintf(buf2, " (value: %u)", object->ob_price);
			strncpy(&buf[loc], buf2, 160 - loc);
			loc += strlen(buf2);
		}
	}
	return(buf);
}
