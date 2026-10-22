/*		printdmg
		84/08/01. 	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/monster.h"
#include "^^def/user.h"

printdmg(player, damage, brf, mask)
/* Be sure to include mask parameter */
struct	us_user	*player;
word	damage;
boolean	brf;
unsigned	mask;
{
	register	struct	us_user	*plyr;

	plyr = validusr(player);

	if (brf) {
		if (plyr->us_fatigue == 0)
			mwrite(mask, "%d vit!\n", damage);
		else
			if (damage > plyr->us_fatigue)
				mwrite(mask, "%d fat, and %d vit!\n", plyr->us_fatigue,
					damage - plyr->us_fatigue); 
			else
				mwrite(mask, "%d fat!\n", damage);
	}
	else {
		if (plyr->us_fatigue == 0)
			mwrite(mask, "%d vitality points!\n", damage);
		else
			if (damage > plyr->us_fatigue)
				mwrite(mask, "%d fatigue pts, and %d vitality pts!\n",
					plyr->us_fatigue, damage - plyr->us_fatigue);
			else
				mwrite(mask, "%d fatigue points!\n", damage);
	}
}
