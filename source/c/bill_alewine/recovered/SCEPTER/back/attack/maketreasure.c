/*		maketreasure.c
		84/08/02.	A. E. Klietz

		Copyright (c) 1984 by Alan E. Klietz.    All Rights Reserved.
*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/monster.h"
#include "^^def/object.h"
#include "^^def/room.h"

extern	word	objlistlen;

extern	word	(*objindex)[6];

maketreasure(monster, rm)
struct	mo_monster	*monster;
word	rm;
{
	boolean	nothing;
	register	struct	ob_object	*obj, *lastobj;
	register	i;
	word	numobjs, obnum;
	float	factor;

	nothing = TRUE;
	mwrite(My_mask, "On %s you find:\n", pm(monster));
	if (monster->mo_objecttail != NULL) {
		nothing = FALSE;
		lastobj = monster->mo_objecttail;
		while (lastobj->ob_next != NULL)
			lastobj = lastobj->ob_next;
		lastobj->ob_next = room[rm].ro_rmobjecttail;
		lastobj = lastobj->ob_next;
		room[rm].ro_rmobjecttail = monster->mo_objecttail;
		monster->mo_objecttail = NULL;
	}
	else
		lastobj = room[rm].ro_rmobjecttail;

	if (monster->mo_whichobj >= objlistlen) {
		mwrite(My_mask, desc(2149), monster->mo_whichobj);
		/* Sce2149 - Object list index %d out of bounds!\n */
		monster->mo_whichobj = 0;
	}

	if (monster->mo_whichobj > 0) {
		if (ran(4) == 1)
			numobjs = ran(4);
		else
			numobjs = 1;
	
		for (i=0; i < numobjs; ++i) {
			obnum = objindex[monster->mo_whichobj][ran(6)-1];
			if (obnum > 0) {
				unsigned	kludge;

				nothing = FALSE;
				obj = get_olist(obnum);
				factor = (ran(40) + 80) / 100.0;
				kludge = obj->ob_price;
				obj->ob_price = (unsigned) (kludge * factor);
				obj->ob_next = room[rm].ro_rmobjecttail;
				room[rm].ro_rmobjecttail = obj;
			}
		}
	}
	if (nothing)
		mwrite(My_mask, "   Nothing.\n");
	else {
		obj = room[rm].ro_rmobjecttail;
		do {
			char	*string;
		
			string = printobj(obj, FALSE);
			string[0] = toupper(string[0]);
			mwrite(My_mask, "   %s\n", string);
			obj = obj->ob_next;
		} while (obj != lastobj);
	}
}
