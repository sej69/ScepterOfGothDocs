/*		Userfile.c - Process user i/o to eperson file.
		84/06/09.	A. E. Klietz.

		Copyright (c) 1984, Alan E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"
#include "^^def/object.h"

#define		MAX_ENTRIES  7  /* Maximum number of player entries per userindex */
						    /* This cannot be raised higher than 7! */

struct	ud_user_directory {
	word	ud_maxentries;  /* Maximum entries allowed.  0 < x <= MAX_ENTRIES */
	char	ud_name[MAX_ENTRIES][12];  /* Name of character entry. */
};

	
static	unsigned	eperson;  /* File descriptor */


odestroy(object)
struct	ob_object	*object;
{
	register	struct	ob_object	*cont, *obj;

	cont = object;
	if (cont->ob_obclass == CHEST) 
		while ((obj = cont->ob.ob_chest.ob_objecttail) != NULL) {
			cont->ob.ob_chest.ob_objecttail = obj->ob_next;
			odestroy(obj);
		}
	free(cont->ob_name);
	free(cont);
}
		
			


readplayer(player, name, userindex)
struct	us_user	*player;
char	*name;
unsigned	userindex;
{
	register	struct	us_user	*usr;
	register	i;
	word		len, indx;
	struct	ud_user_directory	user_directory;
	register	struct	ob_object	*obj, *obj2;
	word	numobjs;
	char	*tmpusr;

/* Player indexes are stored in the eperson file in the format

      5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    /    user index            / indx /
*/

	usr = validusr(player);

	if (eperson == 0)
		if ((eperson = random_open("/data/scepter/eperson")) == 0)
			crash(desc(700));
			/* Sce700 - Unable to open eperson file! */

	if (random_read(eperson, userindex << 3, &user_directory,	
			sizeof(user_directory)) < sizeof(user_directory)) {
		/* If no user directory found. */
		
		usr->us_name[0] = '\0';
		return;
	}

	/* search directory for name match */

	for (i=0; i < MAX_ENTRIES; ++i)
		if (equal(user_directory.ud_name[i], name))
			break;

	if (i >= MAX_ENTRIES) { /* If player not found */
		usr->us_name[0] = '\0';
		return;
	}
		
	indx = i;
	i += (userindex << 3) + 1;
	if ((len = random_allocread(eperson, i, &tmpusr, 5000))
		== 0) { /* if record is zero but directory is not */
			user_directory.ud_name[indx][0] = '\0';		
			random_write(eperson, userindex << 3, &user_directory, 
				sizeof(user_directory));
			usr->us_name[0] = '\0';
			return;
		}

	copy(usr, tmpusr, sizeof(struct us_user));
	i = sizeof(struct us_user);
	if (i > len)
		crash(desc(702));
		/* Sce702 - user record truncated in eperson! */

	usr->us_weight = 0;
	usr->us_objecttail = NULL;
	usr->us_ac = 10;
	usr->us_next = NULL;
	usr->us_usweap = NULL;
	usr->us_usarm = NULL;
	usr->us_defplayer = NULL;
	usr->us_usshield = NULL;
	usr->us_defmon = NULL;
	usr->us_entry = XNAME;
	usr->us_status = SLOGIN;
	time(&usr->us_lastatk);
	time(&usr->us_lastheal);
	time(&usr->us_drunk);
	usr->us_lastcmd = "";

	if (i < len) { /* if first object */
		obj = alloc(sizeof(struct ob_object));
		numobjs = 1;
		usr->us_objecttail = obj;
		copy(obj, &tmpusr[i], sizeof(struct ob_object));
		usr->us_weight = obj->ob_weight;
		i += sizeof(struct ob_object);
		obj->ob_name = alloc(strlen(&tmpusr[i])+1);
		strncpy(obj->ob_name, &tmpusr[i], BUFLEN);
		i += strlen(&tmpusr[i])+1;
		while (i < len && numobjs < MAXOBJS) { /* if next object */
			obj2 = alloc(sizeof(struct ob_object));
			obj->ob_next = obj2;
			copy(obj2, &tmpusr[i], sizeof(struct ob_object));
			i += sizeof(struct ob_object);
			obj2->ob_name = alloc(strlen(&tmpusr[i])+1);
			strncpy(obj2->ob_name, &tmpusr[i], BUFLEN);
			i += strlen(&tmpusr[i])+1;
			++numobjs;
			obj = obj2;
			{
				word	kludge;

				kludge = usr->us_weight;
				usr->us_weight = kludge + obj->ob_weight;
			}
		}
		obj->ob_next = NULL;
	}
	zero(tmpusr, len);
	free(tmpusr);
}




writeusr(pt)
/* erase user record. */
struct	us_user	*pt;
{
	register	struct	us_user	*usr;
	register	struct	ob_object	*object, *object2;

	usr = validusr(pt);
	object = usr->us_objecttail;
	zero(usr, sizeof(struct us_user));
	free(usr);
	if (object != NULL)  /* erase objects */
		do {
			object2 = object->ob_next;
			odestroy(object);
			object = object2;
		} while (object != NULL);
}



boolean writeplayer(player, name, userindex, erase)
/* Write player record.  Return TRUE if successful. */
struct	us_user	*player;
char	*name;
unsigned	userindex;
boolean	erase;  /* TRUE if record should be freed from memory */
{
	register	struct	us_user	*usr;
	register	i;
	register	struct	ob_object	*object;
	word		len, indx;
	struct	ud_user_directory	user_directory;
	word	numobjs;
	char	*tmpusr;

	if (erase != TRUE && erase != FALSE)
		crash(desc(593));
		/* Sce593 - writeplayer() erase flag not TRUE or FALSE!\n */

	usr = validusr(player);

	if (random_read(eperson, userindex << 3, &user_directory,	
			sizeof(user_directory)) < sizeof(user_directory)) {
		/* If no user directory found. */
		
		zero(&user_directory, sizeof(user_directory));
		user_directory.ud_maxentries = 2; /* default */
		if (random_write(eperson, userindex << 3, &user_directory,
				sizeof(user_directory)) < sizeof(user_directory)) 
			crash(desc(701));
			/* Sce701 - Unable to create new user_directory! */
	
		mwrite(My_mask, desc(702), userindex);
		/* New user directory created for ui=%d.\n */
	}

	/* search directory for name match */

	for (i=0, len = 0; i < MAX_ENTRIES; ++i) {
		if (user_directory.ud_name[i][0] != '\0')
			++len;
		if (equal(user_directory.ud_name[i], name))
			break;
	}

	if (i >= MAX_ENTRIES) { /* If player not found */
		for (i=0; i < MAX_ENTRIES; ++i) /* search for open slot */
			if (user_directory.ud_name[i][0] == '\0')
				break;

		if (i >= MAX_ENTRIES || len >= user_directory.ud_maxentries) {
			mwrite(My_mask, desc(705), user_directory.ud_maxentries);
			/* You exceeded your stored character limit of %d.\n */
			writeusr(usr);
			return(FALSE);
		}
	}

	indx = i + (userindex << 3) + 1;

	if (usr->us_name[0] == '\0') { /* if record to be deleted */
		random_write(eperson, indx, tmpusr, 0);  /* erase */
		writeusr(usr);  /* free user */
		mwrite(My_mask, desc(706));
		/* Player entry erased. */
		return(TRUE);
	}

	if (!equal(user_directory.ud_name[i], usr->us_name)) {
		strncpy(user_directory.ud_name[i], usr->us_name, 12);
		random_write(eperson, userindex << 3, &user_directory, 
			sizeof(user_directory));  /* update directory entry */
	}


	tmpusr = alloc(sizeof(struct us_user));
	i = sizeof(struct us_user);
	copy(tmpusr, usr, i);
	if ((object = usr->us_objecttail) != NULL) 
		do {
			word	temp, temp2;
			
			if (object->ob_obclass == CHEST) {
				temp2 = object->ob.ob_chest.ob_numinside;
				object->ob.ob_chest.ob_numinside = 0;
				temp = object->ob.ob_chest.ob_objecttail;
				object->ob.ob_chest.ob_objecttail = NULL;
			}
			free(tmpusr);
			tmpusr = realloc(tmpusr, i + sizeof(struct ob_object)
				+ 1 + strlen(object->ob_name));
			copy(&tmpusr[i], object, sizeof(struct ob_object));
			i += sizeof(struct ob_object);
			strncpy(&tmpusr[i], object->ob_name, BUFLEN);
			i += strlen(object->ob_name) + 1;
			if (object->ob_obclass == CHEST) {
				object->ob.ob_chest.ob_objecttail = temp;
				object->ob.ob_chest.ob_numinside = temp2;
			}
		} while ((object = object->ob_next) != NULL);
	
	if (random_write(eperson, indx, tmpusr, i) != i)
		crash(desc(707));
		/* Sce707 - Unable to write user record!\n */

	zero(tmpusr, i);
	free(tmpusr);
	if (erase)
		writeusr(usr);
	mwrite(My_mask, desc(708));
	/* Player file updated.\n */
	return(TRUE);
}


boolean show_userfile(userindex)
unsigned		userindex;
{
	boolean		found = FALSE;
	struct	ud_user_directory	user_directory;
	register	i;

	if (random_read(eperson, userindex << 3, &user_directory,	
			sizeof(user_directory)) < sizeof(user_directory)) 
		return(FALSE);

	
	for (i=0; i < MAX_ENTRIES; ++i)
		if (user_directory.ud_name[i][0] != '\0') {
			found = TRUE;
			mwrite(My_mask, "  %s\n", user_directory.ud_name[i]);
		}

	return(found);
}


word	set_numentries(userindex, n)
/* Return new num of entries.  n=-1 erase,  n=-2 query */
unsigned	userindex;
word		n;
{
	struct	ud_user_directory	user_directory;
	register	i;

	if (random_read(eperson, userindex << 3, &user_directory,
			sizeof(user_directory)) < sizeof(user_directory)) {
		mwrite(My_mask, "No such ui.\n");
		return(-1);
	}

	if (n == -2)
		return(user_directory.ud_maxentries);

	if (n == -1) {
		for (i=0; i < MAX_ENTRIES; ++i)
			if (user_directory.ud_name[i][0] != '\0')
				random_write(eperson, i + (userindex << 3) + 1, 0, 0);
		random_write(eperson, userindex << 3, 0, 0);
		mwrite(My_mask, "All records for ui=%d erased.\n", userindex);
		return(-1);
	}

	n = max(0, min(n, MAX_ENTRIES));	

	user_directory.ud_maxentries = n;
	if (random_write(eperson, userindex << 3, &user_directory,
		sizeof(user_directory)) < sizeof(user_directory)) 
			crash(desc(707));
	
	return(n);
}
