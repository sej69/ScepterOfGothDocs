/*		whydie - enter obituary for player.
		84/08/08.  A. E. Klietz.

	Obituaries are stored in indexes 251 to 253.
The day of the month for the last time the game was brought
up is stored in index 259.  The new obituaries are
stored in indexes 261 to 263.  */


#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

/* The following definitions are duplicated in buy.c and update.c */

#define NUM_OBITS   3   /* Number of obituaries in the paper */
#define OLD_OBIT	251 /* Base index for yesterday's obits */
#define	NEW_OBIT	261 /* Base index for today's obits */
#define MONTHDAY	259 /* Day-of-month for yesterday's obits */


extern	word	obit_lvl[NUM_OBITS];
extern	char	*cname[];

static	char	*adverb[] = {
"unexpectedly",
"suddenly",
"mysteriously",
"believed to have been",
"reportedly",
};

static	char	*verb[] = {
"slain",
"killed",
"found dead",
"discovered slain",
"vanquished in combat",
};

whydie()
{
	register	struct	us_user	*usr;
	register	i,j;

	usr = validusr(user);

	/*  Remove player from game */

	usr->us_status = SLOGIN;
	deleteplayer(usr, usr->us_rmcode);
	j = 0;  /* Find lowest level obit */
	for (i=1; i < NUM_OBITS; ++i)
		if (obit_lvl[i] < obit_lvl[j])
			j = i;

	if (obit_lvl[j] < user->us_lvl) { /* If player earns an obit */
		usr->us_entry = XDEAD;  /* "Please enter an obit..." */
		promptuser();
	}
	else
		dead();  /* Standard death handler */
}


enterwhydie()
{
	register	struct	us_user	*usr;
	register	char	*string;
	char		clstring[BUFLEN], line1[160];
	register	i,j;

	if (strlen(buffer) <= 5) /* If obit isnt long enough */
		return;  /* ask again */

	j=0;  /* Find lowest level obit */
	for (i=1; i < NUM_OBITS; ++i)
		if (obit_lvl[i] < obit_lvl[j])
			j = i;

	if (obit_lvl[j] < usr->us_lvl) { /* Obit confirmed */
		if ((i = strlen(buffer)) > 159)
			buffer[159] = '\0';

		/* Write the obit */
		obit_lvl[j] = usr->us_lvl;

		strncpy(clstring, cname[usr->us_class], 20);
		clstring[0] = toupper(clstring[0]);

		tsprintf(line1, "%.12s, the %.20s level %.20s, was %.30s %.30s yesterday.",
			genuser[usr->us_trm].gn_name, pnth(obit_lvl[j]), clstring,
			adverb[ran(5)-1], verb[ran(5)-1]);

		string = alloc(strlen(line1) + i + 30);
		tsprintf(string, "%2d,%s\n%s\n", obit_lvl[j], line1, buffer);
		writedesc(j + NEW_OBIT, string);
		free(string);
	}		
	else  /* Obit preempted by someone else */
		mwrite(My_mask, desc(2734));
		/* On second thought, forget it!\n */

	dead(); /* Standard death handler */
}
