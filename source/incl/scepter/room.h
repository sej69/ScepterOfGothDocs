/*			room.h - Scepter room definitions
			84/06/04.  A. E. Klietz.
*/

#include "/std/def.h"
#include "^^def/user.h"
#include "^^def/monster.h"
#include "^^def/object.h"

struct	ro_room {
	word		ro_adjoin[7];		/* adjoining rooms 0-4095 */
	word		ro_desccode;		/* room description code 0-32767 */
	bitfield	ro_whichencounter : 8; /* monster encounter type */ 
	bitfield	ro_encountertime : 10; /* seconds between encounters */
	bitfield	ro_notifydm : 1;	/* TRUE if DM notified on entry */
	bitfield	ro_safe : 1;		/* TRUE if room is safe haven */
	bitfield	ro_lastdir : 4;		/* last known direction */
 struct us_user 	*ro_rmplayertail;	/* link to user list */
 struct mo_monster  *ro_rmmonstertail;  /* link to monster list */
 struct	ob_objectr  *ro_rmobjecttail;   /* link to object list */
};



struct	new_room {
	word		new_adjoin[7];		/* adjoining rooms 0-4095 */
	word		new_desccode;		/* room description code 0-32767 */
	bitfield	new_whichencounter : 8; /* monster encounter type */ 
	bitfield    new_version : 2;        /* Room version code */
	bitfield	new_encountertime : 10; /* seconds between encounters */
	bitfield	new_notifydm : 1;	/* TRUE if DM notified on entry */
	bitfield	new_safe : 1;		/* TRUE if room is safe haven */
	bitfield	new_lastdir : 4;		/* last known direction */
 struct us_user 	*new_rmplayertail;	/* link to user list */
 struct mo_monster  *new_rmmonstertail;  /* link to monster list */
 struct	ob_objectr  *new_rmobjecttail;   /* link to object list */
};


struct ro_room  *room;

struct new_room *newroom;

main()
{
	room = alloc(sizeof(struct ro_room));	
	newroom = alloc(sizeof(struct new_room));

	tfprintf(stdout, "%d, %d.\n", sizeof(struct ro_room), sizeof(struct new_room));

	room->ro_whichencounter = 170;
	room->ro_encountertime = 623;

	newroom->new_whichencounter = 170;
	newroom->new_encountertime = 623;

	tfprintf(stdout, "%d %d %d %d", room->new_whichencounter,
		room->new_encountertime, newroom->ro_whichencounter,
		newroom->ro_encountertime);

	tfprintf(stdout, "%d", newroom->new_version);
}
