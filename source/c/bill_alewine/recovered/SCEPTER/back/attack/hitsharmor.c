/*		hitsharmor.c
		84/08/01. 	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/user.h"

hitsharmor(player, damage)
/* be sure to call with &damage pointer. */
struct	us_user	*player;
word	*damage;  /* pointer */
{
	register	struct	us_user 	*plyr;
	register	struct	ob_object	*temp;

	plyr = validusr(player);
	if (plyr->us_usarm != NULL) {
		word	kludge;

		temp = plyr->us_usarm;
		*damage = max(0, (int) (*damage - temp->ob.ob_armor.ob_armplus));
		kludge = temp->ob.ob_armor.ob_armhitsleft; 
		temp->ob.ob_armor.ob_armhitsleft = max(0, kludge - 1);
		if (kludge - 1 <= 0) {
			mwrite(1 << plyr->us_trm, desc(1297));
			/* Your armor falls apart!\n */
			if (deleteobject(temp, plyr->us_objecttail))
				plyr->us_objecttail = plyr->us_objecttail->ob_next;
			kludge = plyr->us_weight;
			plyr->us_weight = max(0, (int) (kludge - temp->ob_weight));
			stopusing(plyr, temp);
			odestroy(temp);
		}
	}

	if (plyr->us_usshield != NULL) {
		word	kludge;

		temp = plyr->us_usshield;
		kludge = temp->ob.ob_shield.ob_shhitsleft;
		temp->ob.ob_shield.ob_shhitsleft = max(0, kludge - 1);
		if (kludge - 1 <= 0) {
			mwrite(1 << plyr->us_trm, desc(1298));
			/* Your shield breaks in half!\n */
			if (deleteobject(temp, plyr->us_objecttail))
				plyr->us_objecttail = plyr->us_objecttail->ob_next;
			kludge = plyr->us_weight;
			plyr->us_weight = max(0, (int) (kludge - temp->ob_weight));
			stopusing(plyr, temp);
			odestroy(temp);
		}
	}
}
