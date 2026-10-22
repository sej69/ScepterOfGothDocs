/*		matchdelete.c - Match things and delete things.
		84/06/09.	A. E. Klietz.                

		Copyright (c) 1984, Alan E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"
#include "^^def/monster.h"
#include "^^def/object.h"
#include "^^def/room.h"



struct	mo_monster	*matchmonster(monstertail, test)
struct	mo_monster	*monstertail;
boolean	(*test)();  /* Pointer to function returning boolean */
{
	register	struct	mo_monster	*pointer;

	for (pointer = monstertail; pointer != NULL; pointer = pointer->mo_next)
		if ((*test)(pointer))
			break;

	return(pointer);
}



struct	ob_object	*matchobject(objecttail, test)
struct	ob_object	*objecttail;
boolean	(*test)();  /* Pointer to function returning boolean */
{
	register	struct	ob_object	*pointer;

	for (pointer = objecttail; pointer != NULL; pointer = pointer->ob_next)
		if ((*test)(pointer))
			break;

	return(pointer);
}



struct	us_user	*matchplayer(playertail, test)
struct	us_user	*playertail;
boolean	(*test)();  /* Pointer to function returning boolean */
{
	register	struct	us_user	*pointer;

	for (pointer = playertail; pointer != NULL; pointer = pointer->us_next)
		if ((*test)(pointer))
			break;

	return(pointer);
}



struct	us_user	*matchuser(usrtail, test)
struct	us_user	*usrtail;
boolean	(*test)();  /* Pointer to function returning boolean */
{
	register	struct	us_user	*pointer;

	for (pointer = usrtail; pointer != NULL; pointer = pointer->us_nextuser)
		if ((*test)(pointer))
			break;

	return(pointer);
}





static	char	*name;

static	boolean	namemon(mon)
struct	mo_monster	*mon;
{
	return(equal(name, mon->mo_name));
}


fixmoncount(monstertail, nam)
/* Restore the index numbers of monsters and one leaves
   or arrives.  Update  ->top and ->nul */
struct	mo_monster	*monstertail;
char	*nam;
{
	register	struct	mo_monster	*pt;
	register	i, nummon;

	name = nam;
	if ((pt = matchmonster(monstertail, namemon)) != NULL) {
		nummon = 0;
		while (pt != NULL) {
			pt = matchmonster(pt->mo_next, namemon);
			++nummon;
		}

		pt = matchmonster(monstertail, namemon);
		for (i=nummon; i >= 1; --i) {
			pt->mo_num = min(i, 9);
			pt->mo_top = (i == nummon);
			pt = matchmonster(pt->mo_next, namemon);
		}
	}
}



static	struct	us_user	*player;

static	boolean	patk(plyr)
struct	us_user	*plyr;
{
	return(plyr->us_defplayer == player);
}

static	boolean	mattack(mon)
struct	mo_monster	*mon;
{
	return(mon->mo_defplayer == player);
}

stopotheratk(playr, rm)
/* stop attack against player that is leaving */
struct	us_user	*playr;
word	rm;
{
	register	struct	us_user	*otherplayer;	
	register	struct	mo_monster	*othermonster;

	player = validusr(playr);
	player->us_defmon = NULL; /* stop player's attacks too */
	player->us_defplayer = NULL;
	otherplayer = matchplayer(room[rm].ro_rmplayertail, patk);
	while (otherplayer != NULL) {
		otherplayer->us_defplayer = NULL;
		otherplayer = matchplayer(otherplayer->us_next, patk);
	}

	othermonster = matchmonster(room[rm].ro_rmmonstertail, mattack);
	while (othermonster != NULL) {
		othermonster->mo_defplayer = NULL;
		if (othermonster->mo_mreact >= 5)
			othermonster->mo_mreact = 0;
		othermonster = matchmonster(othermonster->mo_next, mattack);
	}
}

	
static	struct	mo_monster	*monster;

static	boolean	pattacking(othrplyr)
struct	us_user	*othrplyr;
{
	return(othrplyr->us_defmon = monster);
}

stopplyratk(mnster, rm)
/* stop attack against a monster that is leaving */
struct	mo_monster	*mnster;
word	rm;
{
	register	struct	us_user	*otherplayer;

	monster = mnster;
	otherplayer = matchplayer(room[rm].ro_rmplayertail, pattacking);
	while (otherplayer != NULL) {
		otherplayer->us_defmon = NULL;
		otherplayer = matchplayer(otherplayer->us_next, pattacking);
	}
}



destroy(mon)
/* destroy a monster */
struct	mo_monster	*mon;
{
	register	struct	ob_object	*obj;

	while ((obj = mon->mo_objecttail) != NULL) {
		mon->mo_objecttail = obj->ob_next;
		odestroy(obj);
	}
	free(mon->mo_name);
	free(mon);
}


insertmonster(monster, rm)
/* add a monster to the tail, call fixmoncount */
struct	mo_monster	*monster;
word	rm;
{
	monster->mo_next = room[rm].ro_rmmonstertail;
	room[rm].ro_rmmonstertail = monster;
	fixmoncount(monster, monster->mo_name);
}



deletemonster(pt, rm)
struct	mo_monster	*pt;
word	rm;
{
	register	struct	mo_monster	*pt2;

	if (pt == NULL)
		crash(desc(783));
		/* Sce783 - Cannot delete null monster!\n */

	if ((pt2 = room[rm].ro_rmmonstertail) == pt)
		room[rm].ro_rmmonstertail = pt2->mo_next;
	else {
		for (; pt2 != NULL; pt2 = pt2->mo_next)
			if (pt2->mo_next == pt)
				break;
		
		if (pt2 == NULL)
			crash(desc(784));
			/* Sce784 - Deleted monster not found!\n */
		else
			pt2->mo_next = pt->mo_next;
	}
	fixmoncount(room[rm].ro_rmmonstertail, pt->mo_name);
	if (room[rm].ro_rmplayertail != NULL)
		stopplyratk(pt, rm);
}


boolean deleteobject(pt, objecttail)
struct	ob_object	*pt, *objecttail;
{
	register	struct	ob_object	*pt2;

	if (pt == NULL)
		crash(desc(785));
		/* Sce785 - Cannot delete null object!\n */

	if  ((pt2 = objecttail) == pt)
		return(TRUE);
	else {
		for (; pt2 != NULL; pt2 = pt2->ob_next)
			if (pt2->ob_next == pt)
				break;

		if (pt2 == NULL)
			crash(desc(786));
			/* Sce786 - Deleted object not found!\n */
		else
			pt2->ob_next = pt->ob_next;

		return(FALSE);
	}
}


cleanrm(rm)
/* Remove old monsters and objects that are not permanent */
word	rm;
{
	register	struct	ob_object	*obj;
	struct		ob_object	*obj2;
	register	struct	mo_monster	*mon;
	struct		mo_monster	*mon2;

	for (obj = room[rm].ro_rmobjecttail; obj != NULL; )
		if (!obj->ob_permanent) {		
			if (deleteobject(obj, room[rm].ro_rmobjecttail))
				room[rm].ro_rmobjecttail = room[rm].ro_rmobjecttail->ob_next;
			obj2 = obj->ob_next;
			odestroy(obj);
			obj = obj2;
		}
		else
			obj = obj->ob_next;
		
	for (mon = room[rm].ro_rmmonstertail; mon != NULL; )
		if (!mon->mo_permanent) {
			mon2 = mon->mo_next;
			deletemonster(mon, rm);
			destroy(mon);
			mon = mon2;
		}
		else {
			mon->mo_hits = mon->mo_maxhits;
			mon = mon->mo_next;
		}
}


deleteplayer(pt, rm)
struct	us_user	*pt;
word	rm;
{
	register	struct	us_user	*pt2;

	if (pt == NULL)
		crash(desc(787));
		/* Sce787 - Cannot delete null player!\n */

	if ((pt2 = room[rm].ro_rmplayertail) == pt) {
		pt2 = room[rm].ro_rmplayertail = pt2->us_next;
		if (pt2 == NULL || pt2->us_hits == 0)
			cleanrm(rm);
	}
	else {
		for (; pt2 != NULL; pt2 = pt2->us_next)
			if (pt2->us_next == pt)
				break;

		if (pt2 == NULL)
			crash(desc(788));
			/* Sce788 - Deleted player not found!\n */
		else
			pt2->us_next = pt->us_next;
	}
	
	stopotheratk(pt, rm);
	pt->us_follow = NULL;
	inactive(rm);
}



deleteuser(pt)
struct	us_user	*pt;
{
	register	struct	us_user	*pt2, *followplyr;
	register	kludge;

	pt = validusr(pt);
	if (pt == NULL || usertail == NULL)
		crash(desc(789));
		/* Sce789 - Cannot delete NULL user!\n */

	if ((pt2 = usertail) == pt)
		usertail = pt2->us_nextuser;
	else {
		for (; pt2 != NULL; pt2 = pt2->us_nextuser)
			if (pt2->us_nextuser == pt)
				break;

		if (pt2 == NULL)
			crash(desc(790));
			/* Sce790 - Deleted user not found!\n */
        else
			pt2->us_nextuser = pt->us_nextuser;
	}

	if (pt->us_status != SLOGIN)
		deleteplayer(pt, pt->us_rmcode);
	
/* KLUDGE DUE TO COMPILER BUG WITH BIT FIELDS */
	kludge = pt->us_rmcode;
	pt->us_rmcode = w(kludge);

	for (followplyr = usertail; followplyr != NULL; followplyr = followplyr->us_nextuser)
		if (followplyr->us_follow == pt)
			followplyr->us_follow = NULL;
}



stopusing(user, object)
struct	us_user	*user;
struct	ob_object	*object;
{
	register	struct	us_user		*usr;

	usr = validusr(user);
	if (object != NULL) {
		if (object == usr->us_usweap)
			usr->us_usweap = NULL;
		else
			if (object == usr->us_usshield) {
				usr->us_usshield = NULL;
				usr->us_ac += object->ob.ob_shield.ob_shplus;
			}
			else
				if (object == usr->us_usarm) {
					usr->us_usarm = NULL;
					usr->us_ac += object->ob.ob_armor.ob_armplus;
				}
	}	
}
