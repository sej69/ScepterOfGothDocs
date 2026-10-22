/*		dead - standard death handler.
		84/08/08.  A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/object.h"
#include "^^def/user.h"
#include "^^def/room.h"

dead()
{
	word		rm;
	register	struct	ob_object	*obj;
	register	struct	us_user		*usr;
	struct		ob_object	*obj2;
	struct		us_user		*body;
	char		string[BUFLEN];
	word		kludge;
	word		lost, i;

	usr = validusr(user);
	rm = usr->us_rmcode;
	
	/* Drop all objects */
	obj = usr->us_objecttail;
	usr->us_objecttail = NULL;
	usr->us_weight = 0;
	usr->us_ac = 10;

	/* Create dead body in room */
	body = alloc(sizeof(struct ob_object));
	tsprintf(string, "body,%.12s's %%s", genuser[usr->us_trm].gn_name);
	body->ob_name = alloc(strlen(string) + 1);
	strcpy(body->ob_name, string);
	body->ob_weight = usr->us_str * 2;
	body->ob_obclass = MISC;
	body->ob_article = NONE;
	
	if (usr->us_con * 5 >= ran(100)) /* If resurrected */
		lost = 1;
	else {
		if (usr->us_lvl >= 5)
			daymsg("Con", "Con failed", usr->us_con + 1);
		mwrite(My_mask, fmt, printdesc(2737, 0, FALSE));
		/* /Your constitution failed./Constitution roll failed./ */
		lost = usr->us_lvl / 2;
	}

	usr->us_usarm = NULL;
	usr->us_usshield = NULL;
	usr->us_usweap = NULL;
	usr->us_entry = XCMD;
	usr->us_status = SNORMAL;
	placeplayer(user, 17);
	usr->us_poisoned = FALSE;
	kludge = usr->us_con;
	usr->us_con = max(3, kludge - lost);
	usr->us_hits = usr->us_maxhits;
	usr->us_fatigue = 5;
	mwrite(My_mask, desc(2735));
	/* \nEverything goes dark... You feel yourself floating away into\n
		 Space.  After what seems like an eternity, you faintly hear\n
		 Voices!  As you come to your senses, you find.......\n\n\n */
	if (usr->us_lvl > 1) {
		kludge = usr->us_maxhits;
		usr->us_maxhits = max(3, kludge - lost * kludge / usr->us_lvl);
		usr->us_hits = usr->us_maxhits;
		kludge = usr->us_maxfatigue;
		usr->us_maxfatigue = max(3, kludge - lost * kludge / usr->us_lvl);
		kludge = usr->us_maxmagic;
		usr->us_maxmagic = max(3, kludge - lost * kludge / usr->us_lvl);
		usr->us_magic = usr->us_maxmagic;
		kludge = usr->us_lvl;
		usr->us_lvl = kludge - lost;

		for (i=0; i < lost; ++i)
			switch (ran(4)) {
case 1: kludge = usr->us_str;  usr->us_str = max(3, kludge - 1); break;
case 2: kludge = usr->us_int;  usr->us_int = max(3, kludge - 1); break;
case 3: kludge = usr->us_dex;  usr->us_dex = max(3, kludge - 1); break;
case 4: kludge = usr->us_con;  usr->us_con = max(3, kludge - 1); break;
		}
	}

	roomdisplay(usr->us_rmcode, FALSE);
	usr->us_money = max(3, usr->us_pty) * 3;
	usr->us_experience = 0;
	mwrite(My_mask, desc(2736));
	/* The acolyte gives you a small bag of money and bids you go\n
	   on your way.\n\n */
	usr->us_dead = FALSE;
	savechar();
	promptuser();

#ifdef  UNDEFINED
	else { /* resurrection failed */
		if (usr->us_lvl >= 5)
			daymsg("Era", "Con failed", usr->us_con + 1);
		mwrite(My_mask, fmt, printdesc(2737, 0, FALSE));
		/* /Your constitution failed./Constitution roll failed./ */
		usr->us_dead = FALSE;
		genuser[usr->us_trm].gn_logout = TRUE;
		kill();
		user = NULL;
	}
#endif


	body->ob_next = room[rm].ro_rmobjecttail;
	room[rm].ro_rmobjecttail = body;		
	obj2 = obj;
	if (obj != NULL) {
		while (obj2->ob_next != NULL)
			obj2 = obj2->ob_next;

		obj2->ob_next = body->ob_next;
		body->ob_next = obj;
	} 
}        
