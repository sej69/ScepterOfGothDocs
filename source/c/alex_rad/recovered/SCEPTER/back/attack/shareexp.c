/*		shareexperience.c
		84/08/01. 	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/monster.h"
#include "^^def/user.h"
#include "^^def/room.h"

shareexperience(mon, rm)
struct	mo_monster	*mon;
word	rm;
{
	register	numinvolved;
	word	newexp;
	register	struct	us_user	*plyr;

	numinvolved = 0;
	plyr = room[rm].ro_rmplayertail;
	while (plyr != NULL) {
		if (plyr->us_defmon == mon)
			++numinvolved;
		plyr = plyr->us_next;
	}
	++numinvolved; /* Add an additional share for the slayer */
	plyr = room[rm].ro_rmplayertail;
	for (;;) {
		while (plyr != NULL) {
			if (plyr->us_defmon == mon)
				break;
			plyr = plyr->us_next;
		}
		if (plyr == NULL)
			break;

		if (plyr == user) 
			plyr->us_experience += ((long) mon->mo_experience) * 2 / numinvolved;
		else {
			newexp = mon->mo_experience / numinvolved;
			if (notimeout(plyr))
				mwrite(1 << plyr->us_trm, "You earned %d experience points.\n",
					newexp);
			plyr->us_experience += newexp;
		}
		plyr = plyr->us_next;
	}
}				
