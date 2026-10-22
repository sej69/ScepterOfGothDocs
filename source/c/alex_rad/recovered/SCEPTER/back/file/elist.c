/*		elist - I/O for OLIST and MLIST.
		84/08/02.	A. E. Klietz.

		Copyright (c) 1984, Alan E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/monster.h"
#include "^^def/object.h"

unsigned	elist;

static	char	*thing = "thing";

struct	ob_object	*get_olist(n)
/*  Return nth object in olist */
{
	register	struct	ob_object	*obj;
	char	*buf, *string;

	obj = alloc(sizeof(struct ob_object));

	if (n <= 0 || n > 4095 || 
			random_allocread(elist, n+10000, &buf, 5000)
		< sizeof(struct ob_object)) { /* If no object */
		string = thing;
		obj->ob_name = alloc(strlen(string)+1);
		strncpy(obj->ob_name, string, BUFLEN);
		obj->ob_obclass = MISC;
		obj->ob_carry = TRUE;
		obj->ob_weight = 1;

		obj->ob_version = 1;
	}
	else {
		word	obj_size;

		obj_size = sizeof(struct ob_object);

		copy(obj, buf, sizeof(struct ob_object));

		if (obj->ob_version != 1)
			obj_size += 6;

		obj->ob_name = alloc(strlen(&buf[obj_size])+1);
		strncpy(obj->ob_name, &buf[obj_size], BUFLEN);
		free(buf);
	}
	obj->ob_next = NULL;
	return(obj);
}



struct	mo_monster	*get_mlist(n)
/*  Return nth monster in mlist */
{
	register	struct	mo_monster	*mon;
	char	*buf, *string;

	mon = alloc(sizeof(struct mo_monster));

	if (n <= 0 || n > 4095 || 
			random_allocread(elist, n+20000, &buf, 5000)
			< sizeof(struct mo_monster)) { /* If no monster */
		string = thing;
		mon->mo_name = alloc(strlen(string)+1);
		strncpy(mon->mo_name, string, BUFLEN);
		mon->mo_defend = TRUE;
		mon->mo_atklastaggr = TRUE;
		mon->mo_lvl = 1;
		mon->mo_hits = 10;
		mon->mo_maxhits = 10;
		mon->mo_top = TRUE;
		mon->mo_num = 1;
	}
	else {
		copy(mon, buf, sizeof(struct mo_monster));
		mon->mo_name = alloc(strlen(&buf[sizeof(struct mo_monster)])+1);
		strncpy(mon->mo_name, &buf[sizeof(struct mo_monster)], BUFLEN);
		free(buf);
	}
	mon->mo_next = NULL;
	return(mon);
}


write_olist(obj, n)
/* Write obj at nth position in olist */
struct	ob_object	*obj;
word	n;
{
	register	char	*buf;
	word		len;

	if (n < 0 || n > 4095) {
		mwrite(My_mask, desc(1230));
		/* Olist write out of bounds.\n */
		return;
	}

	if (equal(obj->ob_name, thing)) {
		random_write(elist, n+10000, 0, 0);
		return;
	}


	obj->ob_version = 1;

	buf = alloc(len = sizeof(struct ob_object) + strlen(obj->ob_name) + 1);
	copy(buf, obj, sizeof(struct ob_object));
	strncpy(&buf[sizeof(struct ob_object)], obj->ob_name, BUFLEN);
	if (random_write(elist, n+10000, buf, len) != len)
		crash(desc(1240), n);
		/* Sce1240 - Failed to write olist at %d!\n */
	free(buf);
}


write_mlist(mon, n)
/* Write mon at nth position in mlist */
struct	mo_monster	*mon;
word	n;
{
	register	char	*buf;
	word		len;

	if (n < 0 || n > 4095) {
		mwrite(My_mask, desc(1250));
		/* Mlist write out of bounds.\n */
		return;
	}

	if (equal(mon->mo_name, thing)) {
		random_write(elist, n+20000, 0, 0);
		return;
	}

	buf = alloc(len = sizeof(struct mo_monster) + strlen(mon->mo_name) + 1);
	copy(buf, mon, sizeof(struct mo_monster));
	strncpy(&buf[sizeof(struct mo_monster)], mon->mo_name, BUFLEN);
	if (random_write(elist, n+20000, buf, len) != len)
		crash(desc(1251), n);
		/* Sce1251 - Failed to write mlist at %d!\n */
	free(buf);
}
