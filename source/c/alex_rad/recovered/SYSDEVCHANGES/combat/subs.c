/*			subs - general combat subroutines 
			84/05/06.	A. E. Klietz.

*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

word	cparseloc; /* location of next character in buffer for parsing. */

#define  DAY_TOP 254
#define  DAY_BOTTOM 255
#define  DAY_START 1
#define  DAY_LIMIT 21

static 	word	day_top, day_bottom;  /* top and bottom of dayfile */
static	FILE	*day_fp;

daymsg(i, reason, by)
/* record dayfile entry for ship[i] with "reason" killed by "by".
   (by = -1 if no killer) */
word	i;
char	*reason;
word	by;
{
	char	buffer[256];

	if (i < 0 || i >= MAX_SHIPS) {
		errlog("warn:daymsg - bad ship #%d\n",i);
		return;
	}

	if (day_bottom >= DAY_LIMIT)
		day_bottom = DAY_START;
	else
		++day_bottom;
	
	random_write(day_fp, DAY_BOTTOM, &day_bottom, sizeof(day_bottom));

	if (day_top == day_bottom) {
		if (day_top >= DAY_LIMIT)
			day_top = DAY_START;
		else
			++day_top;
		random_write(day_fp, DAY_TOP, &day_top, sizeof(day_top));
	}

	if (by < 0)
		tsprintf(buffer, "%2d  %-9s, %11s %s %3d %03u-%04d  %s",
			i, cdate(), ctime(),
			reason, ship[i].sh_kills, (unsigned) genuser[i].gn_useracct,
			genuser[i].gn_userindex, genuser[i].gn_name);
	else
		tsprintf(buffer, "%2d  %-9s, %11s %s%-2d %2d %03u-%04d  %s",
			i, cdate(), ctime(), 
			reason, by, ship[i].sh_kills, (unsigned) genuser[i].gn_useracct,
			genuser[i].gn_userindex, genuser[i].gn_name);

	buffer[79] = '\0';
	random_write(day_fp, day_bottom, buffer, 80);
}
			

show_dayfile()		
{
	char	buffer[80];
	word	index;
	
	index = day_bottom;
	if (!user->us_brief)
		mwrite(My_mask, "\nSp     Date      Time      Sta  Kl Act-Index Commander\n");

	while (index != day_top) {
		if (random_read(day_fp, index, buffer, 80) <= 0) {
			errlog("dayfile:warn - bad index read %d\n",index);
			break;
		}
		mwrite(My_mask,"%s\n", buffer);
		if (index <= DAY_START)
			index = DAY_LIMIT;
		else
			--index;
	}
	mwrite(My_mask,"\n");
}



day_init()
{
	if ((day_fp = random_open("/data/combat/dayfile")) == 0) /* if cannot open */
		if ((day_fp = random_create("/data/combat/dayfile", 4096, 0)) == 0)
			crash("fatal:combat - unable to create dayfile.\n");
		else { /* init dayfile */
			day_top = day_bottom = DAY_START;
			random_write(day_fp, DAY_TOP, &day_top, sizeof(day_top));
			random_write(day_fp, DAY_BOTTOM, &day_bottom, sizeof(day_bottom));
			random_write(day_fp, day_top, "Dayfile initialized.", 80);
		}
    else {
		random_read(day_fp, DAY_TOP, &day_top, sizeof(day_top));
		random_read(day_fp, DAY_BOTTOM, &day_bottom, sizeof(day_bottom));
	}
}
	
			
 
double mag(i, j)
/* Find the magnitude of the given vector pair. */
double		i,	j;
{
	return(sqrt(i*i + j*j));
}


double azmuth(we, they)
/* find azmuth of we-ship to they-ship */
word	we,	they;
{
	struct	sh_ship		*weptr, *theyptr;
	double	i_vec, j_vec, m, th;

	if (we < 0 || we >= MAX_SHIPS || they < 0 || they >= MAX_SHIPS
			|| we == they) {
		errlog("warn:azmuth - bad we/they parameter.\n");
		return(0.0);
	}

	weptr = &ship[we];

	if (weptr->sh_good_azmuth[they]) /* if already computed */
		return(weptr->sh_azmuth[they]);

	theyptr = &ship[they];

	i_vec = theyptr->sh_disp_i - weptr->sh_disp_i;
	j_vec = theyptr->sh_disp_j - weptr->sh_disp_j;
	m = mag(i_vec, j_vec);
	i_vec /= m;
	j_vec /= m;
	if (fabs(m = i_vec * weptr->sh_bow_i + j_vec * weptr->sh_bow_j) >= 1)
		th = 0;
	else {
		th = acos(m);
	    if (j_vec * weptr->sh_bow_i < i_vec * weptr->sh_bow_j)
			th = -th;
	}

	/* update azmuth computation */
	weptr->sh_good_azmuth[they] = TRUE;
	weptr->sh_azmuth[they] = th;
	return(th);
}



kill_credit(dead_ptr, kill_ptr)
/* Give kill_ptr credit for the death of dead_ptr */
struct	sh_ship	*dead_ptr, *kill_ptr;
{
	kill_ptr->sh_power += dead_ptr->sh_power + 5000L;
	if (kill_ptr->sh_kills % 5 == 4) /* if 5th kill */
		kill_ptr->sh_missles = 25;
	kill_ptr->sh_kills += 1;
	kill_ptr->sh_repair_wait = 0;
}



cisseparator(ch)
/* Procedure isseparator  - return true if ch is one of the
characters " ,?%#" */
word	ch;
{
	char	*delim = " ,%#";
	register pos, ch2;

	pos = 0;
	while ((ch2 = delim[pos++]) != '\0')
		if (ch == ch2)
			return(1);
	
	return(0);
}


cstring_tolower(string)
/* Procedure string_tolower - convert a string to lowercase. */

char	*string;
{
	register	pos;

	pos = 0;
	while (string[pos] != '\0') {
		string[pos] = tolower(string[pos]);
        ++pos;
	}
}


creset_parse()
/* Procedure reset_parse - reset the parse counter. */

{
	cparseloc = 0;
}


cupper_parse(buffer, wrd)
/* Procedure parse - extract a single word from 'buffer' to 'word'.
The size of 'word' must equal the size of 'buffer' to avoid over-
flowing the word string.    The procedure returns the length of the
word, zero if end-of-string. Words are not* converted to lowercase. */

char	*buffer, *wrd;
{
	word		wordpos;  /* character pointer for 'word'.*/
	register	ch;

	if (cparseloc == -1) { /* end of buffer already encountered. */
		wrd[0] = '\0'; /* guarantee end of string. */
		return(0);
	}

	while ((ch = buffer[cparseloc++]) != '\0')
		if (!cisseparator(ch)) /* skip separators. */
			break;

	if (ch == '\0') { /* return zero if no non-seps found. */
		cparseloc = -1;
        wrd[0] = '\0'; /* guarantee end of string. */
		return(0);
	}

	wordpos = 0;
	if (isdigit(ch) || ch == '-' || ch == '.')  /* fill number */
		do {  /* fill word */
			wrd[wordpos++] = ch;
	    	ch = buffer[cparseloc++];
		} while ((isdigit(ch) || ch == '.') && wordpos < BUFLEN - 1);
    else
		do {  /* fill word */
			wrd[wordpos++] = ch;
	    	ch = buffer[cparseloc++];
		} while ((ch != '\0') && !cisseparator(ch) &&
			(!isdigit(ch) && ch != '.' && ch != '-') &&
			wordpos < BUFLEN - 1);

	--cparseloc;
	wrd[wordpos] = '\0'; /* add trailing zero to make proper string. */

	if (ch == '\0') /* end of buffer? */
		cparseloc = -1;
	else {
		while ((ch = buffer[cparseloc++]) != '\0')
			if (!cisseparator(ch)) /* skip separators. */
				break;
		--cparseloc;
		if (ch == '\0')
			cparseloc = -1;
	}

	return(wordpos);
}
	


cparse(string, wrd)
/* Parse - parse string into lowercase words.  
This procedure is identical to upper_parse, except it converts words
to lowercase. */

char	*string, *wrd;
{
	word	len;

	len = cupper_parse(string, wrd);
	cstring_tolower(wrd);  /* convert to lowercase. */
	if (strlen(wrd) == 2)
		if (equal(wrd, "on") ||
			equal(wrd, "at") ||
			equal(wrd, "to") ||
			equal(wrd, "of"))
			return(cparse(string, wrd));

	return(len);
}


dparse(string, wrd)
/* parse and reset if parameter is not a digit */
char	*string, *wrd;
{
	register	i,len;

	i = cparseloc;
	len = cparse(string, wrd);
	if (wrd[0] >= 'a' && wrd[0] <= 'z') {
		cparseloc = i;
		return(0);
	}
	return(len);
}
