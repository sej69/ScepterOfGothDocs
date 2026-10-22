/*			set - combat set command
			84/05/06.	A. E. Klietz.

*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

static struct cm_setlist {
	char	*cm_command;
	word	cm_index;
}	setlist[] = {
	"f", 8,
	"full shields", 8,
	"g",	5,
	"p",	3,
	"range", 5,
	"repair", 3,
	"s",	4,
	"shield", 4,
	"tracking", 10,
	"v",	10,
	"z", 9,
	"zero shields", 9,
};

#define  SETLIST_LEN	(sizeof(setlist) / sizeof(struct cm_setlist))

set(buffer)
char	*buffer;
{
	struct	sh_ship		*shptr;
	char	wrd[BUFLEN], wrd2[BUFLEN];	
	word	setnum;
	double	f,g;
	word	i,j;
	extern	word	cparseloc;

	shptr = &ship[My_ship];

	cparse(buffer, wrd);

	setnum = binsearch_ptr(wrd, &setlist[0].cm_command,		
		sizeof(struct cm_setlist), SETLIST_LEN);
	if (setnum >= 0) /* convert to index */
		setnum = setlist[setnum].cm_index;

	switch (setnum) {

case -2: mwrite(My_mask,"\"%s\" is not unique.\n", wrd);
		 break;

case -1: mwrite(My_mask, "\"%s\" - no such SET parameter.\n", wrd);
		mwrite(My_mask, "\nValid SET parameters are:\n");
		for (i=0; i < SETLIST_LEN; ++i)
			if (strlen(setlist[i].cm_command) > 1)
				mwrite(My_mask,"%s\n", setlist[i].cm_command);
		mwrite(My_mask,"\n");
	 	break;


case 8: /* full */
	shptr->sh_shield[0].sh_pwr = 25;
	shptr->sh_shield[1].sh_pwr = 25;
	if (!user->us_brief)
		mwrite(My_mask,"Shields set to full power.\n");

	i = cparseloc;
	cparse(buffer, wrd);
	if (!subset(wrd, "shields"))
		cparseloc = i;
	break;

case 3: /* repair */
	if (dparse(buffer,wrd) <= 0 || dparse(buffer, wrd2) <= 0) {
		if (!user->us_brief)
			mwrite(My_mask,"Missing repair percentages:  set repair s1%%,s2%%\n");
		break;
	}

	if ((i = atoi(wrd)) < 0 || i > 100 || (j = atoi(wrd2)) < 0 || j > 100) {
		if (!user->us_brief)
			mwrite(My_mask,"Bad repair percetanges.  0 <= %% <= 100\n");
		break;
	}
	
	if (i+j > 100)	{
		if (!user->us_brief)
			mwrite(My_mask,"Percentages > 100%%.  Average used.\n");
		i /=(i+j);
		j /=(i+j);
	}
	shptr->sh_ship_repair = 100 - i - j;
	shptr->sh_shield[0].sh_repair = i;
	shptr->sh_shield[1].sh_repair = j;
	if (!user->us_brief)
		mwrite(My_mask,"Repair distribution - main ship: %d%%.  Shield#1: %d%%.  Shield#2: %d%%.\n",
			100 - i - j, i, j);
	break;

case 9: /* zero */
	shptr->sh_shield[0].sh_pwr = 0;
	shptr->sh_shield[1].sh_pwr = 0;
	if (!user->us_brief)
		mwrite(My_mask,"Shields turned off.\n");
	i = cparseloc;
	cparse(buffer, wrd);
	if (!subset(wrd, "shields"))
		cparseloc = i;
	break;

case 10: /* vector */
	if (dparse(buffer, wrd) <= 0) {
		if (!user->us_brief)
			mwrite(My_mask, "Tracking turned off.\n");
		user->us_vector = -1; /* disable vector */
		break;
	}

	if ((i = atoi(wrd)) < 0 || i >= MAX_SHIPS || i == My_ship
			|| !ship[i].sh_exist) {
		if (!user->us_brief)
			mwrite(My_mask, "Bad ship vector.\n");
		user->us_vector = -1;
		break;
	}
	if (!user->us_brief)
		mwrite(My_mask, "Tracking ship #%d.\n", i);
	user->us_vector = i;
	break;




case 5: /* display */
{ 	longword	l;

	if (dparse(buffer, wrd) <= 0 || (l = atol(wrd)) <= 0) {
		if (!user->us_brief)
			mwrite(My_mask, "Bad or missing display range.\n");
		break;
	}

	if (!user->us_brief)
		mwritef(My_mask, "Display range set to %ldkm.\n", l);
	user->us_display_range = l;
	break;		
}


case 4: /* shield */
	if (dparse(buffer,wrd) <= 0 || (i = atoi(wrd)) < 1 || i > 2) {
		if (!user->us_brief)
			mwrite(My_mask,"Missing or bad shield number (Must be 1 or 2).\n");
		break;
	}

	if (dparse(buffer,wrd) <= 0 || (j = atoi(wrd)) < 0 || i > 25) {
		if (!user->us_brief)
			mwrite(My_mask,"Missing or bad shield power (0 <= power <= 25).\n");
		break;
	}

	shptr->sh_shield[i-1].sh_pwr = j;
	if (!user->us_brief)
		mwrite(My_mask,"Shield #%d set to %d units power.\n",i,j);
	break;
	

default:
	mwrite(My_mask, "%s - parameter not implemented yet.\n", wrd);
	break;

		}
}
