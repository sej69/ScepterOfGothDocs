/*		hitmonster.c
		84/08/02.	A. E. Klietz

		Copyright (c) 1984 by Alan E. Klietz.    All Rights Reserved.
*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"

hitmonster(damage)
word	damage;
{
	register	struct	mo_monster	*monster;
	register	struct	us_user		*usr;
	word		rm;
	register	i;

	usr = validusr(user);
	rm = usr->us_rmcode;
	monster = usr->us_defmon;
	if (((int) monster->mo_hits) - damage <= 0) {
		mwrite(My_mask, "You killed the %s!\n", monster->mo_name);
		if (Local_mask != 0)
			mwrite(Local_mask, "%s just killed %s!\n", genuser[usr->us_trm].gn_name,
				pm(monster));
		shareexperience(monster, rm);
		maketreasure(monster, rm);
		deletemonster(monster, rm);
		destroy(monster);
		usr->us_defmon = NULL;
	}
	else {
		if ((i = monster->mo_mparley) == 1 || i == 3 || i == 4)
			if (!monster->mo_permanent)
				monster->mo_mparley = 2;
		i = monster->mo_hits;
		monster->mo_hits = max(0, i - damage);
		if (usr->us_brief)
			mwrite(My_mask, "You hit it for %d hit points!\n", damage);
		else
			mwrite(My_mask, "You hit the %s for %d hit points!\n", monster->mo_name,
				damage);
		if (monster->mo_flee || usr->us_lvl >= 5 + monster->mo_lvl)
			if (((float) monster->mo_hits) / monster->mo_maxhits < 0.01 * ran(40)) {
				if (Local_mask != 0) {
					char	*string;

					string = pm(monster);
					string[0] = toupper(string[0]);
					mwrite(Local_mask, "%s flees from %s's attack!\n", string,
						genuser[usr->us_trm].gn_name);
				}
				mwrite(My_mask, "The %s flees from your attack!\n",
					monster->mo_name);
				monster->mo_experience = monster->mo_experience / 2;
				if (usr->us_lvl < 5 + monster->mo_lvl)
					shareexperience(monster, rm);
				deletemonster(monster, rm);
				destroy(monster);
				usr->us_defmon = NULL;
			}
	}
}



hitatmonster(weapon, plus)
struct	ob_object	*weapon;
word	plus;
{
	register	struct	ob_object	*weap;
	register	struct	mo_monster	*defmon;
	word		damage;
	boolean		withfist;

	defmon = user->us_defmon;
	if (defmon->mo_nokill) {
		user->us_defmon = NULL;
		mwrite(My_mask, desc(838), defmon->mo_name);
		/* Aw, don't attack the poor %s.\n */
		return;
	}

	weap = weapon;
	withfist = FALSE;
	if (weapon == NULL) {
		withfist = TRUE;
		weapon = get_fist();
		weap = weapon;
	}
	if (defmon->mo_magic && !weap->ob_magic) 
		mwrite(My_mask, desc(2455));
		/* Your weapon strikes with no effect!\n */
	else {
		if (user->us_class == PALADIN)
			if (defmon->mo_slowreact || defmon->mo_fastreact)
				++plus;
		if (user->us_class == FIGHTER)
			++plus;
		if (user->us_lvl == 1)
			plus += 2;
		if (defmon->mo_invisible)
			plus -= 4;
		if (defmon->mo_defend && defmon->mo_defplayer == NULL
			|| defmon->mo_atklastaggr && defmon->mo_defplayer != user) {
			defmon->mo_defplayer = user;
			if (defmon->mo_mreact >= 5)
				defmon->mo_mreact = 0;
		}
		if (!defmon->mo_permanent) {
			register	i;

			if ((i = defmon->mo_mparley) == 1 || i == 3 || i == 4)
				defmon->mo_mparley = 2;
		}
		if (ran(20) != 20 && ran(20) < 12 + ((int) defmon->mo_lvl)
			- ((int) user->us_lvl)
			- ((int) weap->ob.ob_weap.ob_weaplus)
			- ((int) user->us_str) / 6 - plus)
			if (withfist)
				mwrite(My_mask, "Your fist misses!\n");
			else
				mwrite(My_mask, "You missed!\n");
		else {
			register	i;

			i = weap->ob.ob_weap.ob_strikesleft;
			weap->ob.ob_weap.ob_strikesleft = i - 1;
			damage = max(0, ((int) weap->ob.ob_weap.ob_minhp) +
				ran(((int) weap->ob.ob_weap.ob_maxhp)
				- ((int) weap->ob.ob_weap.ob_minhp) + 1)
				+ ((int) weap->ob.ob_weap.ob_weaplus) + plus);
			if (withfist)
				mwrite(My_mask, "Punch!\n");
			switch (ran(100 - ((int) user->us_lvl))) {
case 1:
case 2:
case 3:
case 4:
				mwrite(My_mask, "Double damage!\n");
				damage *= 2;
				break;

case 90:
				mwrite(My_mask, "You fumbled!\n");
				damage = 0;  user->us_usweap = NULL;
				break;
			}
			if (damage >= defmon->mo_hits)
				if ((((int) (defmon->mo_lvl - user->us_lvl))) * 5 > ran(25)  
					&& !user->us_skillnew) {
					register	i;

					user->us_skillnew = TRUE;
					switch (weap->ob.ob_weap.ob_weaptype) {
case SHARP: if (ran(i = user->us_ssharp) == 1) 
				user->us_ssharp = min(7, i + 1);
			break;


case THRUST: if (ran(i = user->us_sthrust) == 1)
				user->us_sthrust = min(7, i + 1);
			break;

case BLUNT: if (ran(i = user->us_sblunt) == 1)
				user->us_sblunt = min(7, i + 1);
			break;

case LONG: if (ran(i = user->us_slong) == 1) 
				user->us_slong = min(7, i + 1);
			break;
					}
				}
			hitmonster(damage);
        }
	}
	if (withfist)
		odestroy(weap);
}
