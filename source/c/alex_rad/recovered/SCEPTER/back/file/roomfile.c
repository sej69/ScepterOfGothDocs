/*		Roomfile.c - Process room i/o to edunjon file.
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

#define  ROOMSPERSEG	5	/* 5 rooms per segment */


#define		KLUDGE		/* Kludgily convert rooms to new format */




struct	ro_room		room[RMLIMIT];

static	unsigned	edunjon;

		word	slottbl[MAX_PORTS + 1];
static	word	 active[MAX_PORTS + 1];
        
static  inactivescan = 0;  /* end around search */

static	char	*tmpdata = NULL;
static	word	tmploc = 0;
static  word	tmpsize = 0;

word s(rm)
/* Return the physical room (0..RMLIMIT) from the logical
   room number (0..4095).  Load in the room if necessary. */
word	rm;
{
	register	i, segnum;

	if (rm < 0 || rm > 4095)
		crash(desc(710), rm);
		/* Sce710 - Room #%d out of bounds!\n */

	segnum = rm / ROOMSPERSEG;
	i = 0;

	while (i <= MAX_PORTS && slottbl[i] != segnum)
		++i;

	if (i <= MAX_PORTS) /* if segment found */
		return(i * ROOMSPERSEG + rm - segnum * ROOMSPERSEG);

	i = 0;
	while (i <= MAX_PORTS && slottbl[i] > -1)
		++i;

	if (i > MAX_PORTS) { /* if no open slots */
	/* Use an end-around search for an inactive slot */
		i = inactivescan;
		while (active[i] > 0) {
			if (++i > MAX_PORTS)
				i = 0;
			if (i == inactivescan)
				crash(desc(725));
				/* Sce725 - Virtual room space full!\n */
		}
		inactivescan = (i >= MAX_PORTS) ? 0 : i+1;
	}

	if (slottbl[i] > -1) /* if already occupied */
		writeseg(i, slottbl[i], TRUE);

	readseg(i, segnum);
	return(i * ROOMSPERSEG + rm - segnum * ROOMSPERSEG);
}


static	writedata(data, len)
/* copy data to tmpdata, and realloc larger as needed */
/* Be sure to set tmpdata = NULL and tmpsize = tmploc = 0 when
   finished using this procedure. */
char	*data;
word	len;
{
	if (tmpsize == 0) { /* initial allocation */
		tmpdata = alloc(500);
		tmpsize = 500;
	}
		
	while (len > tmpsize - tmploc) { /* allocate larger */
		free(tmpdata);
		tmpdata = realloc(tmpdata, tmpsize + 500);
		tmpsize += 500;
	}

	copy(&tmpdata[tmploc], data, len);
	tmploc += len;
}


static	boolean readdata(data, len)
/* copy tmpdata to data */
/* Be sure to read tmpdata and set tmpsize = size, and tmploc = 0
   BEFORE using this procedure.  Free tmpdata afterwards and set
   tmpsize = tmploc = 0 */
char	*data;
word	len;
{
	copy(data, &tmpdata[tmploc], min(len, tmpsize - tmploc));
	tmploc = min(tmpsize, tmploc + len);
	return(tmploc >= tmpsize);
}


writeseg(slotnum, segnum, erase)
/* write out room segment, optionally erase room data */
word	slotnum, segnum;
boolean	erase;
{
	word	rm, r;
	struct	mo_monster	*monster, *mon2;
 	register	struct	ob_object	*object,  *obj2;

	if (segnum < 0 || segnum > 4095 / ROOMSPERSEG) {
		error_handler(0);
		crash(desc(730), segnum * ROOMSPERSEG);
		/* Sce730 - Room cluster %d out of bounds! */
	}
	
	for (r=0; r < ROOMSPERSEG; ++r) {
		word	k;

		rm = r + ROOMSPERSEG * slotnum;
		for (k=0; k < 7; ++k)
			if (((unsigned) room[rm].ro_adjoin[k]) > 4095) {
				error_handler(0);
				crash(desc(544), r + ROOMSPERSEG * segnum);
				/* Sce544 - Room #%d seems to be trashed.\n */
			}


#ifdef  KLUDGE
		room[rm].ro_version = 1;
#endif

		writedata(&room[rm], sizeof(struct ro_room));
		object = room[rm].ro_rmobjecttail;
		while (object != NULL) {
			writedata(object, sizeof(struct ob_object));
			writedata(object->ob_name, strlen(object->ob_name)+1);
			savechest(object, erase);
			obj2 = object->ob_next;
			if (erase) {
				free(object->ob_name);
				free(object);
			}
			object = obj2;
		}
	
		monster = room[rm].ro_rmmonstertail;
		while (monster != NULL) {
			monster->mo_defplayer = NULL;
			writedata(monster, sizeof(struct mo_monster));
			writedata(monster->mo_name, strlen(monster->mo_name)+1);
			object = monster->mo_objecttail;
			while (object != NULL) {
				writedata(object, sizeof(struct ob_object));
				writedata(object->ob_name, strlen(object->ob_name)+1);
				obj2 = object->ob_next;
				if (erase) {
					free(object->ob_name);
					free(object);
				}
				object = obj2;
			}
			mon2 = monster->mo_next;
			if (erase) {
				free(monster->mo_name);
				free(monster);
			}
			monster = mon2;
		}
	}
	if (random_write(edunjon, segnum, tmpdata, tmploc) != tmploc) {
		error_handler(0);
		crash(desc(731), segnum * ROOMSPERSEG);
		/* Sce731 - Unable to write cluster %d!\n */
	}

	free(tmpdata);
	tmpdata = NULL;
	tmpsize = tmploc = 0;
}



static savechest(cont, erase)
struct	ob_object	*cont;
boolean	erase;
{
	register	struct	ob_object	*obj, *obj2;

	if (cont->ob_obclass == CHEST) {
		obj = cont->ob.ob_chest.ob_objecttail;
		while (obj != NULL) {
			writedata(obj, sizeof(struct ob_object));
			writedata(obj->ob_name, strlen(obj->ob_name)+1);
			savechest(obj, erase);
			obj2 = obj->ob_next;
			if (erase) {
				free(obj->ob_name);
				free(obj);
			}
			obj = obj2;
		}
	}
}



static readname(ptrptr)
char	**ptrptr;
{
	*ptrptr = alloc(strlen(&tmpdata[tmploc])+1);
	readdata(*ptrptr, strlen(&tmpdata[tmploc])+1);
}


static	word	object_size;  


readseg(slotnum, segnum)
word	slotnum, segnum;
{
	word	r, rm;
	register	struct	ob_object	*object, *object2;
	struct	mo_monster	*monster,*monster2;

#ifdef  KLUDGE
	boolean	newrooms;     /* KLUDGE */
#endif

	object_size = sizeof(struct ob_object);

	if (segnum < 0 || segnum > 4095 / ROOMSPERSEG)
		crash(desc(730), segnum * ROOMSPERSEG);
		/* Sce730 - Room cluster %d out of bounds!\n */

	if (edunjon == 0)
		if ((edunjon = random_open("/data/scepter/edunjon")) == 0)
			crash(desc(737));
			/* Sce737 - Unable to open edunjon!\n */

	slottbl[slotnum] = segnum;
	tmploc = 0;

#ifdef  KLUDGE
	newrooms = FALSE;
#endif

	if ((tmpsize = random_allocread(edunjon, segnum, &tmpdata, 5000))
			< sizeof(struct ro_room)) {

#ifdef KLUDGE
		newrooms = TRUE;
#endif
		tmpdata  = alloc(tmpsize = sizeof(struct ro_room) * ROOMSPERSEG);
		mwrite(My_mask, desc(732), segnum * ROOMSPERSEG);
		/* New room cluster %d allocated.\n"); */
	}
		

	for (r = 0; r < ROOMSPERSEG; ++r) {
		rm = r + ROOMSPERSEG * slotnum;
		readdata(&room[rm], sizeof(struct ro_room));
		room[rm].ro_rmplayertail = NULL;

#ifdef  KLUDGE
		if (newrooms)
			room[rm].ro_version = 1;  /* KLUDGE */
		if (room[rm].ro_version != 1)
			object_size = sizeof(struct ob_object) + 6;
#endif

		if (room[rm].ro_rmobjecttail != NULL) {
			object = alloc(object_size);
			room[rm].ro_rmobjecttail = object;
			readdata(object, object_size);
			readname(&object->ob_name);
			getchest(object);
			while (object->ob_next != NULL) {
				object2 = alloc(object_size);
				object->ob_next = object2;
				readdata(object2, object_size);
				readname(&object2->ob_name);
				object = object2;
				getchest(object);
			}
		}
		
		if (room[rm].ro_rmmonstertail != NULL) {
			monster = alloc(sizeof(struct mo_monster));
			room[rm].ro_rmmonstertail = monster;
			readdata(monster, sizeof(struct mo_monster));
			readname(&monster->mo_name);
			monster->mo_defplayer = NULL;
			getmonobjects(monster);
			while (monster->mo_next != NULL) {
				monster2 = alloc(sizeof(struct mo_monster));
				monster->mo_next = monster2;
				readdata(monster2, sizeof(struct mo_monster));
				readname(&monster2->mo_name);
				monster2->mo_defplayer = NULL;
				getmonobjects(monster2);
				monster = monster2;
			}
		}
	}
	if (!readdata(monster, 0)) /* if not EOF */
		crash(desc(736), segnum * ROOMSPERSEG);
		/* Sce736 - Failed to read cluster %d entirely!\n */

	free(tmpdata);
	tmpsize = tmploc = 0;
}



static	getchest(cont)
struct	ob_object	*cont;
{
	register	struct	ob_object	*obj, *obj2;

	if (cont->ob_obclass == CHEST) 
		if (cont->ob.ob_chest.ob_objecttail != NULL) {
			obj = alloc(object_size);
			cont->ob.ob_chest.ob_objecttail = obj;
			readdata(obj, object_size);
			readname(&obj->ob_name);
			getchest(obj);
			while (obj->ob_next != NULL) {
				obj2 = alloc(object_size);
				obj->ob_next = obj2;
				readdata(obj2, object_size);
				readname(&obj2->ob_name);
				obj = obj2;
				getchest(obj);
			}
		}
}



static getmonobjects(monster)
struct	mo_monster	*monster;
{
	register	struct	ob_object	*obj, *obj2;

	if (monster->mo_objecttail != NULL) {
		obj = alloc(object_size);
		monster->mo_objecttail = obj;
		readdata(obj, object_size);
		readname(&obj->ob_name);
		while (obj->ob_next != NULL) {
			obj2 = alloc(object_size);
			obj->ob_next = obj2;
			readdata(obj2, object_size);
			readname(&obj2->ob_name);
			obj = obj2;
		}
	}
}



word sa(rm)
/* Load and set active-room segment */
word	rm;
{
	register	seg, segrm;

	segrm = s(rm);
	seg = segrm / ROOMSPERSEG;
	++(active[seg]);
	return(segrm);
}




inactive(rm)
/* Set room inactive */
word	rm;
{
	register	seg;

	seg = rm / ROOMSPERSEG;
	if (active[seg] <= 0)
		crash(desc(765));
		/* Sce765 - Non active segment assumed active! */
		
	--(active[seg]);               
}


word w(rm)
/* Return logical number (0-4095) of physical room (0..RMLIMIT) */
word	rm;
{
	register	where;

	if (rm < 0 || rm >= RMLIMIT)
		return(0);

	where = slottbl[rm / ROOMSPERSEG] * ROOMSPERSEG + rm
			- (rm / ROOMSPERSEG) * ROOMSPERSEG;

	if (where < 0 || where >= 4096)
		where = 0;

	return(where);
}



placeplayer(plyr, rm)
/* Insert player into logical room */
struct	us_user	*plyr;
word	rm;
{
	register	newrm;
	register	unsigned	mask;
	register	struct	us_user	*player;

	newrm = sa(rm);
	player = plyr->us_next = room[newrm].ro_rmplayertail;
	room[newrm].ro_rmplayertail = plyr;
	plyr->us_rmcode = newrm;
	
	/* Set up the new Local_mask for the new room */
	mask = 0;
	while (player != NULL) {
		mask |= 1 << player->us_trm;
		player = player->us_next;
	}
	
	Local_mask = mask;
}	
