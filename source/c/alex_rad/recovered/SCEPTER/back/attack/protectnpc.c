/*		protectnpc
		84/08/01. 	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"

protectnpc(player, monster)
struct	us_user	*player;
struct	mo_monster	*monster;
{
	register	struct	us_user	*plyr;
	register	struct	ob_object	*temp;
	register	i, mask;

	plyr = validusr(player);
	mask = 1 << plyr->us_trm;
	mwrite(mask, desc(2100), monster->mo_name);
	/* As you step forward to attack, the %s yells */
	mwrite(mask, desc(2101));
	/* "Help! Help! Guards! Guards!"... in seconds several\n */
	mwrite(mask, desc(2102));
	/* burly guards grab you, and take away your weapon!\n */
	if (plyr->us_usweap != NULL) {
		temp = plyr->us_usweap;
		plyr->us_usweap = NULL;
		if (deleteobject(temp, plyr->us_objecttail))
			plyr->us_objecttail = plyr->us_objecttail->ob_next;
		i = plyr->us_weight;
		plyr->us_weight = max(0, (int) (i - temp->ob_weight));
		odestroy(temp);
	}
	if (plyr->us_lvl <= 2) {
		mwrite(mask, desc(2103));
		/* But since you are a young, inexperienced adventurer, they\ngive you a stern warning a decide not to throw you in jail.\n */
		mwrite(Local_mask, desc(2104), genuser[plyr->us_trm].gn_name,
			monster->mo_name, plyr->us_sex ? "her" : "him");
		/* %s assaults the %s, but the town guards stop %s\n */
	}
	else {
		mwrite(mask, desc(2105));
		/* Then they throw you in jail!\n */
		mwrite(Allbutme_mask, desc(2106), genuser[plyr->us_trm].gn_name,
			monster->mo_name);
		/* ### %s was thrown in jail for assaulting the %s.\n */
		deleteplayer(plyr, plyr->us_rmcode);		
		placeplayer(plyr, 5); /* jail */
		mwrite(mask, "\n");
		i = My_mask;
		My_mask = mask;
		roomdisplay(plyr->us_rmcode, FALSE);
		My_mask = i;
	}
	plyr->us_defmon = NULL;
}


struct	ob_object	*selectweap()
{
	register	struct	us_user	*usr;

	usr = validusr(user);
	if (usr->us_usweap == NULL)
		return(NULL);

	if (usr->us_usweap->ob.ob_weap.ob_strikesleft == 0) {
		usr->us_usweap->ob_permanent = FALSE;
		mwrite(My_mask, desc(2110));
		/* Your weapon is broken!\n */
		return(NULL);
	}
	else
		return(usr->us_usweap);
}
