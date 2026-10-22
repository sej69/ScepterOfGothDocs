/*			display - display things.
			84/06/12.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"

extern	char	*cname[];

static	char	*tf[] = {
	"False",
	"True" };


objdisplay(obj)
struct	ob_object	*obj;
{
	register	struct	ob_object	*object, *tobj;
	char		*name;

	object = obj;
	name = printobj(object, FALSE);
	name[0] = toupper(name[0]);
	mwrite(My_mask, "%s, %s%d lbs, %u shillings.\n", name,
		(object->ob_carry ? "carryable, " : ""),
		object->ob_weight,
		object->ob_price);

	if (object->ob_desccode != 0 
			&& (user->us_ssj || object->ob_obclass != SCROLL)) 
		mwrite(My_mask, "Descrip (%d): %s\n", object->ob_desccode,
			printdesc(object->ob_desccode, 0, FALSE));

	
	mwrite(My_mask, "Magic=%s, Permanent=%s, Invisible=%s\n",
		tf[object->ob_magic], tf[object->ob_permanent],
		tf[object->ob_invisible]);

	switch (object->ob_obclass) {
	
		case PORTAL:
			mwrite(My_mask, "Portal to room %d.\n", 
				object->ob.ob_portal.ob_towhere);
			break;

		case WEAP:
			mwrite(My_mask, "Offensive weapon, %d-%d hp, %d strikes left.\n",
				object->ob.ob_weap.ob_minhp, object->ob.ob_weap.ob_maxhp,
				object->ob.ob_weap.ob_strikesleft);
			switch (object->ob.ob_weap.ob_weaptype) {
				case SHARP: mwrite(My_mask, "Sharp ");
				break;
				case THRUST: mwrite(My_mask, "Thrust ");
				break;
				case BLUNT: mwrite(My_mask, "Blunt ");
				break;
				case LONG: mwrite(My_mask, "Pole ");
				break;
			}
			mwrite(My_mask, "class weapon,  +%d to hit.\n", 
				object->ob.ob_weap.ob_weaplus);
			break;
	
		case SHIELD:
			mwrite(My_mask, "Shield, +%d protection,  %d hits left.\n",
				object->ob.ob_shield.ob_shplus, object->ob.ob_shield.ob_shhitsleft);
			break;

		case ARMOR:
			mwrite(My_mask, "Armor, +%d protection,  %d hits left.\n",
				object->ob.ob_armor.ob_armplus, object->ob.ob_armor.ob_armhitsleft);
			break;

		case COINS:
			mwrite(My_mask, "Money, value multiplier: X%d.\n", 
				object->ob.ob_coins.ob_multiplier);
			break;

		case SCROLL:
			mwrite(My_mask, "Scroll, spell = %s\n",
				spellist[object->ob.ob_scroll.ob_spell]);
			break;

		case CHEST:
			mwrite(My_mask, "Container, objects=%d, trap=%d\n",
				object->ob.ob_chest.ob_numinside,
				object->ob.ob_chest.ob_trap);
			mwrite(My_mask, "Lock type = %d.\n", object->ob.ob_chest.ob_locked);
			if ((tobj = object->ob.ob_chest.ob_objecttail) != NULL) {	
				mwrite(My_mask, "Items inside:\n");
				while (tobj != NULL) {
					mwrite(My_mask, "    %s\n", printobj(tobj, FALSE));
					tobj = tobj->ob_next;
				}
			}
			break;

		case DOOR:
			mwrite(My_mask, "Door, portal to room %d, door trap = %d.\n",
				object->ob.ob_door.ob_dtowhere, object->ob.ob_door.ob_dtrap);
			mwrite(My_mask, "Dlock type=%d.\n", object->ob.ob_door.ob_dlocked);
			break;

		case MAGDEVICE:
			mwrite(My_mask, "Magical device, spell = %s.\n",
				spellist[object->ob.ob_magdevice.ob_mspell]);
			mwrite(My_mask, "Number of charges = %d.\n",
				object->ob.ob_magdevice.ob_numcharges);
		break;

		case TELEPORT:
			mwrite(My_mask, "Teleport device, from room %d to room %d.\n",
				object->ob.ob_teleport.ob_tactiverm,
				object->ob.ob_teleport.ob_towhere);
			break;

		case KEYS:
			mwrite(My_mask, "Keys, unlock type=%d.\n",object->ob.ob_keys.ob_unlock);
			break;

		case CARD:
			mwrite(My_mask, "Cards, teleport to player.\n");
			break;

		default:
			mwrite(My_mask, "\n");
			break;
	}
}



longword	expr(lev)
word	lev;
{
	register	num;

	num = lev;

	if (num <= 1)
		return(0L);

	if (num <= 7)
		return ((longword) (1 << (num+7)));

	if (num == 8)
		return(32768L);

	if (num == 9)
		return(65536L);

	if (num == 10)
		return(131072L);

	return(262144L * (longword) (num - 10));
}



playerdisplay(plyer)
struct	us_user	*plyer;
{
	register	count;
	register	struct	us_user	*usr;
	register	struct	ob_object	*obj;
	char		*name;

	usr = validusr(plyer);

	usr->us_name[0] = toupper(usr->us_name[0]);
	mwrite(My_mask,"%s, the %s level %s", 
		usr->us_name, pnth(usr->us_lvl), cname[usr->us_class]);
	usr->us_name[0] = tolower(usr->us_name[0]);
	if (usr->us_playtester)
		mwrite(My_mask, " (Playtester)");
	if (usr->us_ssj && !usr->us_assoc && !usr->us_master)
		mwrite(My_mask, " (DM)");
	if (usr->us_ssj && usr->us_assoc)
		mwrite(My_mask, " (Assoc DM)");
	if (usr->us_ssj && usr->us_master)
		mwrite(My_mask, " (Master DM)");
	if (user->us_ssj)
		mwrite(My_mask, " in room %d/%d", usr->us_rmcode,
			w(usr->us_rmcode));
	mwrite(My_mask, "\n");

	if (usr->us_invisible)
		if (usr->us_nonexistant)
			mwrite(My_mask, desc(801));
			/* You are nonexistant. */
		else
			mwrite(My_mask, desc(800));
			/* You are invisible */
	if (usr->us_tguild)
		mwrite(My_mask, desc(803));
		/* You are a member of the thieves guild. */
	if (usr->us_aguild)
		mwrite(My_mask, desc(804));
		/* You are a member of the assassins guild. */
	if (usr->us_evil)
		mwrite(My_mask, desc(805));
		/* You have fallen into the forces of evil. */
	
	mwrite(My_mask, "with %d/%d Vitality pts, ", usr->us_hits,
		usr->us_maxhits);
	mwrite(My_mask, "%d/%d Fatigue pts, and %d/%d Magic pts.\n",
		usr->us_fatigue, usr->us_maxfatigue, usr->us_magic, usr->us_maxmagic);
	if (usr->us_poisoned)
		mwrite(My_mask, desc(806));
		/* You are dying from poison! */
	mwrite(My_mask, desc(807), pn(usr->us_ac));
		/* You have a defensive armor class of %s.\n */
	mwrite(My_mask, "Str=%d, Int=%d, Dex=%d, Pty=%d,",
		usr->us_str, usr->us_int, usr->us_dex, usr->us_pty);
	mwrite(My_mask, " Con=%d.\n", usr->us_con);
	mwrite(My_mask, "Weapon skill: Sharp-%d%%, Thrust-%d%%, Blunt-%d%%, Pole-%d%%\n",
		usr->us_ssharp * 10, usr->us_sthrust * 10, usr->us_sblunt * 10,
		usr->us_slong * 10);
{
	longword	pts;

	if ((pts = expr(usr->us_lvl + 1) - expr(usr->us_lvl) - usr->us_experience)
		< 0L)
		pts = 0L;

	mwritef(My_mask, desc(808), pts);
	/* You need %ld more exp points to reach the next level. */
}
	mwritef(My_mask, desc(809), usr->us_money);
	/* You have %ld shillings in cash. */
	mwritef(My_mask, desc(810), usr->us_weight);
	/* You are carrying %d lbs of items. */

	count = 0;
	obj = usr->us_objecttail;
	while (obj != NULL) {
		++count;
		name = printobj(obj, FALSE);
		name[0] = toupper(name[0]);
		mwrite(My_mask,"   %d) %s\n", count, name);
		obj = obj->ob_next;
	}
	if (count > MAXOBJS)
		mwrite(My_mask, desc(811), pn(MAXOBJS));
		/* Only the first %s items will be saved when you log out.\n */
}


mondisplay(monster)
struct	mo_monster	*monster;
{
	register	struct	mo_monster	*mon;
	register	struct	ob_object	*object;
	char	*str;

	mon = monster;
	str = pm(mon);
	str[0] = toupper(str[0]);
	mwrite(My_mask, "%s is a %s level monster with %d/%d HP\n", 
		str, pnth(mon->mo_lvl), mon->mo_hits, mon->mo_maxhits);

	mwrite(My_mask, "Defend=%s, Block=%s, Follow=%s,\n",
		tf[mon->mo_defend], tf[mon->mo_block], tf[mon->mo_follow]);
	mwrite(My_mask, "Guard=%s, Atklastaggr=%s, Permanent=%s.\n",
		tf[mon->mo_guard], tf[mon->mo_atklastaggr], tf[mon->mo_permanent]);
	mwrite(My_mask, "%d Experience pts.\n", mon->mo_experience);
	if (mon->mo_moralreact)
		mwrite(My_mask, desc(820));
		/* Monster will hit players with piety less than eight.\n */
	if (mon->mo_invisible)
		mwrite(My_mask, desc(821));
		/* Monster is invisible.\n */
	if (mon->mo_flee)
		mwrite(My_mask, desc(822));
		/* Monster may flee if attacked. */
	if (mon->mo_assistance)
		mwrite(My_mask, desc(823));
		/* Monster will call for help if attacked. */
	if (mon->mo_magic)
		mwrite(My_mask, desc(824));
		/* Affected only by magical weapons.\n */
	if (mon->mo_poison)
		mwrite(My_mask, desc(825));
		/* Monster is poisonous. */
	if (mon->mo_antimagic)
		mwrite(My_mask, desc(826));
		/* Monster is immune to spells.\n */
	if (mon->mo_undead)
		mwrite(My_mask, desc(827));
		/* Monster is undead.\n */
	if (mon->mo_slowreact)
		mwrite(My_mask, desc(828));
		/* Monster will react slowly.\n */
	if (mon->mo_fastreact)
		mwrite(My_mask, desc(829));
		/* Monster will react quickly.\n */
	if (mon->mo_regenerate)
		mwrite(My_mask, desc(830));
		/* Monster can regenerate hp.\n */
	if (mon->mo_drain)
		mwrite(My_mask, desc(831));
		/* Monster can drain levels from players!\n */
	if (mon->mo_monspells)
		mwrite(My_mask, desc(832));
		/* Monster can cast spells.\n */
	if (mon->mo_nokill)
		mwrite(My_mask, desc(839));
		/* Monster should not be killed.\n */
	if (mon->mo_kidnap)
		mwrite(My_mask, desc(1840));
		/* Monster can kidnap players.\n */
	if (mon->mo_mparley >0)
		mwrite(My_mask, desc(833), pn(mon->mo_mparley));
	mwrite(My_mask, "Treasure type: %s.\n", pn(mon->mo_whichobj));

	if ((object = monster->mo_objecttail) != NULL) 	{
		mwrite(My_mask, "\nObjects carried:\n");
		do {
			mwrite(My_mask, "  %s\n", printobj(object, FALSE));
			object = object->ob_next;
		} while (object != NULL);
	}
}
