/*			display2 - display more things.
			84/06/12.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/monster.h"
#include "^^def/user.h"
#include "^^def/room.h"

extern char	*dirlist[];

punctuate(num, total)
{
	if (num != total)
		if (total - num >= 2)
			mwrite(My_mask, ", ");
		else
			mwrite(My_mask, " and ");

	if (num % 3 == 0 && num < total)
		mwrite(My_mask, "\n");
}

inventory()
{
	register	i, count;
	register	struct	ob_object	*object;
	char	*name;

	mwrite(My_mask, desc(840));
	/* You are carrying the following objects:\n */

	count = 0;
	object = user->us_objecttail;
	while (object != NULL) {
		++count;
		object = object->ob_next;
	}

	if (count > 0) {
		object = user->us_objecttail;
		i = 0;
		while (i < count && object != NULL) {
			++i;  
			if (i == 1) {
				name = printobj(object, FALSE);
				name[0] = toupper(name[0]);
				mwrite(My_mask, "%s", name);
			}
			else
				mwrite(My_mask, "%s", printobj(object, FALSE));
			punctuate(i, count);
			object = object->ob_next;
		}
		mwrite(My_mask, ".\n\n");
	}
	else
		mwrite(My_mask, desc(841));
		/* Nothing at all.\n */
}	


roomdisplay(rm, brief)
word	rm;
boolean	brief;
{
	register	i, count;
	register	struct	ob_object	*object;
	register	struct	mo_monster	*monster;
	register	struct	us_user	*player;
	char	*tense[2];

	mwrite(My_mask, "You're %s", printdesc(room[rm].ro_desccode, 0,
		brief));
	if (user->us_ssj)
		mwrite(My_mask, " (%d)", w(rm));
	mwrite(My_mask, "\n");
	count = 0;
	for (i=0; i < 7; ++i)
		if (room[rm].ro_adjoin[i] != 0)
			++count;
	if (count > 0) {
		mwrite(My_mask,"Obvious exits are ");
		for (i=0; i < 7; ++i) 
			if (room[rm].ro_adjoin[i] != 0) {
				mwrite(My_mask, "%s%s", dirlist[i],
					count == 1 ? ".\n" : (count  > 2 ? ", " : " and "));
				--count;
			}
	}

	count = 0;	object = room[rm].ro_rmobjecttail;
	while (object != NULL) {
		if (object->ob_invisible == FALSE || user->us_ssj)
			++count;
		object = object->ob_next;
	}

	monster = room[rm].ro_rmmonstertail;			
	while (monster != NULL) {
		if (monster->mo_top && (monster->mo_invisible == FALSE || user->us_ssj))
			++count;
		monster = monster->mo_next;
	}

	if (count > 0) {
		mwrite(My_mask, "You see ");
		object = room[rm].ro_rmobjecttail;	
		monster = room[rm].ro_rmmonstertail;
		i = 0;
		while (i < count && monster != NULL) {
			if (monster->mo_top && (monster->mo_invisible == FALSE || user->us_ssj))
				if (monster->mo_num == 1) {
					++i;
					mwrite(My_mask, "a %s%s", monster->mo_name,
						monster->mo_invisible && user->us_ssj ? " (inv)" : "");
					punctuate(i, count);
				}
				else {
					++i;
					mwrite(My_mask, "%s %ss%s", pn(monster->mo_num),
						monster->mo_name,
						monster->mo_invisible && user->us_ssj ? " (inv)" : "");
					punctuate(i, count);
				}
			monster = monster->mo_next;
		}

		while (i < count && object != NULL) {
			if (object->ob_invisible == FALSE || user->us_ssj) {
				++i;
				mwrite(My_mask, "%s", printobj(object, FALSE));
				punctuate(i, count);
			}
			object = object->ob_next;
		}

		mwrite(My_mask, ".\n\n");
	}
			
	player = room[rm].ro_rmplayertail;  count = 0;
	while (player != NULL) {
		if (player != user && (player->us_invisible == FALSE) &&	
				(player->us_hidden == FALSE || user->us_ssj))
			++count;
        player = player->us_next;
	}

	if (count > 0) {
		player = room[rm].ro_rmplayertail; 	 i = 0;
		while (player != NULL) {
			if (player != user && player->us_invisible == FALSE &&
					(player->us_hidden == FALSE || (user->us_ssj))) {
				mwrite(My_mask, "%s%s", genuser[player->us_trm].gn_name,
					player->us_hidden ?  " (hid)" : "");
				++i;
				punctuate(i, count);
			}
			player = player->us_next;
		}
        if (count == 1)
			mwrite(My_mask, " is also here.\n");
		else
			mwrite(My_mask, " are also here.\n");
	}

	tense[FALSE] = " is";
	tense[TRUE] = "'re";
	monster = room[rm].ro_rmmonstertail;
	while (monster != NULL) {
		if (monster->mo_defplayer != NULL) {
			char	*string;

			string = pm(monster);
			string[0] = toupper(string[0]);
			mwrite(My_mask, "%s is attacking %s!\n", string,
				monster->mo_defplayer == user ? "you" 
				: genuser[monster->mo_defplayer->us_trm].gn_name);
		}
		monster = monster->mo_next;
	}

	player = room[rm].ro_rmplayertail;
	while (player != NULL) {
		if (player->us_defmon != NULL) {
			if (player->us_defmon->mo_defplayer != player)
				mwrite(My_mask, "%s%s attacking %s!\n", player == user ?
					"You" : genuser[player->us_trm].gn_name,
					tense[player == user], pm(player->us_defmon));
		}
		else
			if (player->us_defplayer != NULL)
				mwrite(My_mask, "%s%s attacking %s!\n", player == user ? 
					"You" : genuser[player->us_trm].gn_name,
					tense[player == user], player->us_defplayer == user ?
					"you" : genuser[player->us_defplayer->us_trm].gn_name);
		player = player->us_next;
	}

}
