/*		attack.c
		84/08/02.	A. E. Klietz

		Copyright (c) 1984 by Alan E. Klietz.    All Rights Reserved.
*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"
#include "^^def/room.h"

static	enum	towhattype {TONONE, TOMONSTER, TOPLAYER} towhat;
static	word	rm;

int	attackwhat(worde)
char	*worde;
{
	register	struct	us_user	*olddefplayer;
	register	struct	mo_monster	*olddefmon;
	register	i;
	char		dummy[BUFLEN];

	olddefplayer = user->us_defplayer;
	user->us_defplayer = findplayer(worde, room[rm].ro_rmplayertail);
	if (user->us_defplayer == user)
		user->us_defplayer = NULL;
	if (user->us_defplayer == NULL) {
		olddefmon = user->us_defmon;
		if (num == 0)
			getword(dummy, &num);
		user->us_defmon = findmonster(worde, num, room[rm].ro_rmmonstertail);
		if (user->us_defmon == NULL) {
	     	if (findobject(worde, num, room[rm].ro_rmobjecttail) != NULL)
				mwrite(My_mask, desc(2345));
				/* Use the command 'smash' instead.\n */
			else
				mwrite(My_mask, "%s isn't here.\n", worde);
			return(TONONE);
		}
		else {
			if (olddefmon != user->us_defmon)
				if (Local_mask != 0)
					mwrite(Local_mask, "%s attacks %s!\n",
						genuser[user->us_trm].gn_name, pm(user->us_defmon));
			return(TOMONSTER);
		}
	}
	else {
		if (olddefplayer != user->us_defplayer) {
			i = 1 << user->us_defplayer->us_trm;
			mwrite(i, "%s is attacking you!\n",
				genuser[user->us_trm].gn_name);
			if ((Local_mask & ~i) != 0)
				mwrite(Local_mask & ~i, "%s attacks %s!\n",
					genuser[user->us_trm].gn_name, 
					genuser[user->us_defplayer->us_trm].gn_name);
		}
		return(TOPLAYER);
	}
}
	

attack(cmdcode, rmnum)
word	cmdcode, rmnum;
{
	word	plus;
	register	struct	ob_object	*weapon;

	rm = rmnum;
    towhat = TONONE;
	weapon = selectweap();
	if (readycheck(&user->us_lastatk) && (weapon != NULL
		|| user->us_usweap == NULL)) {
		if (room[rm].ro_safe)
			mwrite(My_mask, desc(2346));
			/* This is a safe haven.\n */
		else 
			if (wrd[0] == '\0') 
				if (user->us_defmon != NULL || user->us_defplayer != NULL) 
					if (user->us_defmon != NULL)
						towhat = TOMONSTER;
					else
						towhat = TOPLAYER;
				else
					mwrite(My_mask, desc(2347));
					/* Specify target, please!\n */
			else
				towhat = attackwhat(wrd);
		if (towhat != TONONE)
			if (cmdcode == 42 || user->us_fatigue > 0) {
				plus = 0;
				switch (cmdcode) {
case 11:			plus = user->us_fatigue;
					user->us_fatigue = max(0, plus - ran(2) + 1);
					plus = 0;
					break;

case 42: /* parry */ plus = -((int) user->us_lvl / 2 + 1);
					if (towhat == TOMONSTER)
						user->us_defmon->mo_mreact = 5; /* half damage */
					break;

case 43: /* thrust*/ plus = user->us_fatigue;
					user->us_fatigue = max(0, (int) (plus - user->us_lvl*2));
					if (towhat == TOMONSTER)
						if (ran(3) == 3) {
							user->us_defmon->mo_mreact = 6; /* double */
							mwrite(My_mask, "You are vulnerable!\n");
						}
					plus = min(user->us_lvl, 4);
					break;

case 85: /* feint */ mwrite(My_mask, "Ok\n"); 
					 if (user->us_dex > ran(30))
						if (towhat == TOMONSTER)
							user->us_defmon->mo_mreact = 3;
						else {
							addtime(&realtime, 10, &(user->us_defplayer->us_lastatk));
							mwrite(1 << user->us_defplayer->us_trm, "%s feints at you!\n",
								genuser[user->us_trm].gn_name);
							mwrite(My_mask, "%s falls back!\n",
								genuser[user->us_defplayer->us_trm].gn_name);
						}
					  else
						mwrite(My_mask, "Didn't work!\n");
					  towhat = TONONE;
					  break;

case 86: /* circle */ mwrite(My_mask, "Ok\n");
					if (user->us_dex > ran(20))
						if (towhat == TOMONSTER)
							user->us_defmon->mo_mreact = 2;
						else {
							addtime(&realtime, 8, &(user->us_defplayer->us_lastatk));
							mwrite(1 << user->us_defplayer->us_trm, "%s circles you!\n",
								genuser[user->us_trm].gn_name);
							mwrite(My_mask, "You circle %s!\n",
								pro[user->us_defplayer->us_sex]);
						}
					else
						mwrite(My_mask, "Didn't work!\n");
					towhat = TONONE;
					break;

case 72: /* backstab */ if (user->us_hidden)
						plus = 5;
					else {
						plus = -5;
						mwrite(My_mask, "You are discovered!\n");
						if (towhat == TOMONSTER) /* double dmg */
							user->us_defmon->mo_mreact = 6;
					}
					break;
				}					
				if (weapon != NULL) 
					switch (weapon->ob.ob_weap.ob_weaptype) {
case SHARP:	 plus += user->us_ssharp; break;
case THRUST: plus += user->us_sthrust; break;
case BLUNT:  plus += user->us_sblunt; break;
case LONG:   plus += user->us_slong; break;
					}
				if (towhat == TOPLAYER)
					addtime(&realtime, 8, &user->us_lastatk);
				else
					addtime(&realtime, 5, &user->us_lastatk);
				user->us_hidden = FALSE;
				if (towhat == TOMONSTER)
					if (user->us_defmon->mo_assistance)
						protectnpc(user, user->us_defmon);
					else
						hitatmonster(weapon, plus);
				else
					if (towhat == TOPLAYER)
						hitatplayer(weapon, plus);
			}
			else 
				mwrite(My_mask, "You're too exhausted to strike!\n");
	}
}
