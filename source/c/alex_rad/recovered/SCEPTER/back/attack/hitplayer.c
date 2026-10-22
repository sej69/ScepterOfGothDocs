/*		hitplayer.c
		84/08/02.	A. E. Klietz

		Copyright (c) 1984 by Alan E. Klietz.    All Rights Reserved.
*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"

extern	char	*cname[];

hitplayer(damage)
word	damage;
{
	register	struct	us_user	*player;
	register	struct	mo_monster *revenent;
	boolean		merc, cold;
	register	i;
	unsigned	plyr_mask;

	player = user->us_defplayer;
	plyr_mask = 1 << player->us_trm;
	hitsharmor(player, &damage);
	mwrite(My_mask, "You hit %s for ", genuser[player->us_trm].gn_name);
	printdmg(player, damage, user->us_brief, My_mask);
	mwrite(plyr_mask, "%s hits you for ", genuser[user->us_trm].gn_name);
	printdmg(player, damage, player->us_brief, plyr_mask);
	if (player->us_hits + player->us_fatigue <= damage)
		if (secdiff(&player->us_hitattime, &realtime) <= 30) {
			mwrite(My_mask, "You killed %s!\n", pro[player->us_sex]);
			mwrite(plyr_mask, "You're dead, %s!\n", genuser[player->us_trm].gn_name);
			merc = (((int) (user->us_lvl - player->us_lvl)) > 3);
			cold = (player->us_defplayer == NULL);
			if (player->us_lvl > 5)
				daymsg("Kil", player->us_name, player->us_lvl);
			mwrite(All_mask, "### %s just killed %s%s%s!\n",
				genuser[user->us_trm].gn_name, genuser[player->us_trm].gn_name,
				merc ? " mercilessly" : "",
				cold ? " in cold blood" : "");
{
			word	kludge;

			kludge = user->us_pty;
			if (merc) 
				kludge = max(-10, kludge - 1);
			if (cold)
				kludge = max(-10, kludge - 1);
			if (user->us_aguild)
				kludge = max(0, kludge - 1);
			user->us_pty = kludge;
		
}
			if (user->us_class == PALADIN && (merc || cold) &&
					player->us_class != THIEF)
				user->us_class = FIGHTER;
			if (player->us_con >= 18) { /* If strong enough for revenent */
				revenent = get_mlist(1);
				insertmonster(revenent, user->us_rmcode);
				revenent->mo_hits = player->us_maxhits;
				revenent->mo_maxhits = player->us_maxhits;
				revenent->mo_lvl = player->us_lvl;
				revenent->mo_defplayer = user;
				mwrite(My_mask, desc(2438));
				/* From the dead body you see a gaseous shape form...\n */
			}
			player->us_dead = TRUE;
			player->us_fatigue = 0;
			player->us_hits = 0;  /* Kill player */
			if ((merc || cold) && (-(((int) user->us_pty) * 10) > ran(100))) {
				mwrite(My_mask, desc(2349));
				/* Enough is enough!  The people rise up against you in an angry mob!\nYou have been lynched!\n */
				daymsg("Lyn", "", 0);
				mwrite(All_mask, desc(2350), genuser[user->us_trm].gn_name,
					cname[user->us_class]);
				/* ### %s, the %s, was just lynched by an angry mob.\n */
				user->us_dead = TRUE;	user->us_hits = 0;
				user->us_fatigue = 0;
			}
		}
		else {
			player->us_fatigue = 0;
			player->us_hits = 1;
			mwrite(My_mask, desc(2351));
			/* You hear a voice cry out, "Run, Fool, Run!"\n */
		}
	else
		if (damage > player->us_fatigue) {
			word	kludge;

			kludge = player->us_hits;
			player->us_hits = max(0, kludge - damage + (int) player->us_fatigue);
			kludge = player->us_fatigue;
			player->us_fatigue = max(0, kludge - damage);
		}
}


struct	ob_object	*get_fist()
{
	register	struct	ob_object	*fist;
	char		*string;

	fist = alloc(sizeof(struct ob_object));
	string = "fist";
	fist->ob_name = alloc(strlen(string)+1);
	strncpy(fist->ob_name, string, BUFLEN);
	fist->ob_obclass = WEAP;
	fist->ob.ob_weap.ob_minhp = 1;
	fist->ob.ob_weap.ob_maxhp = 3;
	fist->ob.ob_weap.ob_strikesleft = 100;
	fist->ob.ob_weap.ob_weaptype = BLUNT;
	fist->ob.ob_weap.ob_weaplus = -2;
	return(fist);
}


hitatplayer(weapon, plus)
struct	ob_object	*weapon;
word	plus;
{
	word	damage;
	boolean	withfist;
	register	struct	us_user	*usr;


	usr = validusr(user);

	withfist = FALSE;
	if (weapon == NULL) {
		withfist = TRUE;
		weapon = get_fist();
	}
	
	if ((ran(20) != 20) && (ran(20) < 24 + ((int) usr->us_defplayer->us_lvl) 
		- ((int) usr->us_lvl) - ((int) weapon->ob.ob_weap.ob_weaplus)
		- ((int) usr->us_str) / 6
		- ((int) usr->us_defplayer->us_ac)
		+ ((int) usr->us_defplayer->us_dex) / 6 - plus)
		|| ((int) usr->us_defplayer->us_hits) == 0) {
		if (withfist)
			mwrite(My_mask, "Woosh!  Your fist misses!\n");
		else
			mwrite(My_mask, "You missed!\n");
		mwrite(1 << usr->us_defplayer->us_trm, "%s misses you!\n",
			genuser[usr->us_trm].gn_name);
	}
	else {
		word	kludge;

		kludge = weapon->ob.ob_weap.ob_strikesleft;
		weapon->ob.ob_weap.ob_strikesleft = kludge - 1;
		damage = max(0, ((int) weapon->ob.ob_weap.ob_minhp) + 
			ran(((int) weapon->ob.ob_weap.ob_maxhp)
			- ((int) weapon->ob.ob_weap.ob_minhp) + 1)
			+ ((int) weapon->ob.ob_weap.ob_weaplus) + plus);
		if (withfist)
			mwrite(My_mask, "Pow!\n");
		hitplayer(damage);
	}
	if (usr->us_defplayer != NULL)
		time(&(usr->us_defplayer->us_hitattime));
	if (withfist)
		odestroy(weapon);
}
