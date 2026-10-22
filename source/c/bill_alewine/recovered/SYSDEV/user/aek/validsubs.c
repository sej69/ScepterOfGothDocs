/*	Validsubs - validation file subroutines.
	83/10/21.	A. E. Klietz.
	
	Hash table added  83/12/25.	

	This is a set of library procedures for manipulating the
/val/validuz and /val/valindx files.  

	The major routines contained are:

	valid_hash(username) - return userindex for username.
					    returns 0 if not found.

	valid_read(userindex, &user_record) - read data into user_record.
						returns 0 if bad read.

	valid_write(userindex, &user_record) - write user_record to file.
						returns 0 if bad write.
	
	valid_create(username) - returns new userindex to open space on file.
						returns 0 if no room, 32767 if duplicate entry.

	valid_delete(userindex) - deletes the entry from valindx.
						returns 0 if already empty.
*/
#include <stdio.h>
#include "aekdef.h"
#include "validef.h"

#define		HASHSEED 		0xD16B



int	hash_index(username)
char	*username;
/* procedure hash_index - return a value from 0 to 255 based on 
'username' */
{
	
	int		hash;
	register	i;

	hash = HASHSEED;  /* set inital value */		
	for (i=0; username[i] != '\0'; i++)   /* scramble */
		hash = ((hash * username[i]) >> 3);

	return(hash & 0xFF); 	/* truncate to 8 bits. */
}


hash_read(fp, user_hash, indx, curloc)
FILE		*fp;
struct		vi_user_hash	*user_hash;
int			indx,	*curloc;
/*	Procedure hash_read - read index-th record from valindx file.
File is assumed already open.  Curloc keeps track of the current 
file position. */
{
	register	i;

	i = *curloc;
	*curloc = (indx-1)*16 + 512;
	fiseek(fp, *curloc - i);
	*curloc += 16;
	if (fget(user_hash, 16, fp) != 16) 
		error("valid:warn - hash index outside of file!\n");
}


hash_write(fp, user_hash, indx, curloc)
FILE		*fp;
struct		vi_user_hash	*user_hash;
int			indx,	*curloc;
/*	Procedure hash_write - write index-th record from valindx file.
File is assumed already open.  Curloc keeps track of the current 
file position. */
{
	register	i;

	i = *curloc;
	*curloc = (indx-1)*16 + 512;
	fiseek(fp, *curloc - i);
	*curloc += 16;
	if (fput(user_hash, 16, fp) != 16) 
		error("valid:warn - unable to write valindx record.\n");
}



unsigned valid_hash(username)
/* Procedure valid_hash - search the /val/valindx file for a userindex
   to match the given username.   Returns zero if not found. */
char	*username;
{
	FILE	*fp;  /* pointer to index file. */
	struct	vi_user_hash	user_hash;
	int  	hashtable[256];
	int		hash, curloc;


	if (sizeof(user_hash) != 16) 
		error("valid:warn - user_hash record is not 16 length!\n");

	if (username[0] == '\0')
		return(0);  /* blank username */

	if ((fp = fopen("/val/valindx", "rv")) == 0)
		return(0);  /* unable to open valindx file. */

	if (fget(hashtable, 512, fp) < 512)  
		error("valid:fatal - missing hash record in valindx!\n");

	curloc = 512;

	hash = hashtable[hash_index(username)];
	if (hash == 0) {
		fclose(fp);
		return(0);
	}

	hash_read(fp, &user_hash, hash, &curloc);
	
	for (;;) {
		if (equal(user_hash.vi_username, username))
			break;
		if (user_hash.vi_link == 0) {
			fclose(fp);
			return(0);	/* name not found */
		}
		hash_read(fp, &user_hash, user_hash.vi_link, &curloc);
	} 


	fclose(fp);

	if (user_hash.vi_userindex == 0) 
		error("valid:warn - user index is zero?\n");

	return(user_hash.vi_userindex);
}


unsigned valid_read(userindex, user_record)
/* Procedure valid_read - read in the user_record pointed by userindex. 
Returns zero if bad read or not found. */
unsigned	userindex;
struct		va_user_record	*user_record;  /* pointer to buffer. */
{
	FILE	*fp;  /* pointer to valid file. */

	if (sizeof(struct va_user_record) != 256) 
		error("valid:warn - user_record is not 256 length!\n");

	if (userindex <= 0 || userindex >= 128) {
		tfprintf(stderr,"valid:warn - userindex out of bounds. ui=%d.\n",userindex);
		return(0);
	}

	if ((fp = fopen("/val/validuz","rv")) == 0)
		return(0);  /* file not found */

	if (fiseek(fp, (userindex-1)*256) != 0) {
		fclose(fp);
		tfprintf(stderr,"valid:warn - offset outside of valid file!\n");
		return(0);
	}

	if (fget(user_record, 256, fp) != 256) {
		fclose(fp);
		tfprintf(stderr,"valid:warn - user_record read past EOF.\n");
		return(0);
	}

	fclose(fp);
	return(1);	/* signal good read. */
}

unsigned valid_write(userindex, user_record)
/* Procedure valid_write - write a user_record to the valid file at userindex. */
unsigned	userindex;
struct	    va_user_record	*user_record; /* pointer to buffer */
{
	FILE	*fp;	/* pointer to valid file. */

    if (userindex <= 0 || userindex >= 128) {
		tfprintf(stderr,"valid:warn - userindex write out of bounds.\n");
		return(0);
	}

	if ((fp = fopen("/val/validuz", "rwv")) == 0)
		return(0);

	if (fiseek(fp, (userindex-1)*256) != 0) {		
		fclose(fp);
		tfprintf(stderr,"valid:warn - write offset outside of valid file!\n");
		return(0);
	}

	if (fput(user_record, 256, fp) != 256) {
		fclose(fp);
		tfprintf(stderr,"valid:warn - write error.\n");
		return(0);
	}

	fclose(fp);
	return(1);	/* signal good write. */
}



unsigned valid_create(username)
/* Procedure valid_create - create an entry in the valindx file and
the valid file. Entries are inserted into a 512 byte hash table that
heads the /val/valindx file. */
char	*username;
{
	FILE	*fp, *fp2;
	struct	va_user_record	user_record;
	struct	vi_user_hash	user_hash;
	int		numread, tableloc, indexloc, recordloc;
	int   	hashtable[256];
	
	if (username[0] == '\0' || valid_hash(username) != 0) 
		return(32767);  /* blank name or duplicate name error. */

	if ((fp = fopen("/val/valindx","rwv")) == 0)			
		return(0);

	if (fget(hashtable, 512, fp) != 512)
		error("valid:fatal - missing hash record in valindx!\n");

	indexloc = 1; /* find open hole in /val/valindx */
	while ((numread = fget(&user_hash, 16, fp)) == 16 &&
			user_hash.vi_username[0] != '\0') 
		indexloc++;

	if (numread == 16)
		fiseek(fp, -16);	/* backspace 1 record */

	
	if ((fp2 = fopen("/val/validuz", "rwv")) == 0) {
		fclose(fp);
		return(0);
	}

	recordloc = 1;
	while ((numread = fget(&user_record, 256, fp2)) == 256 &&	
			user_record.va_userindex != 0)
		recordloc++;
	
	if (numread == 256)  /* found empty slot. */
		fiseek(fp2, -256); /* backspace one record. */

	zero(&user_record, sizeof(user_record));
	strcpy(user_record.va_username, username);
	user_record.va_userindex = recordloc;
	fput(&user_record, 256, fp2);	/* write new record. */
	fclose(fp2);
	zero(&user_hash, sizeof(user_hash));
	strcpy(user_hash.vi_username, username);
	user_hash.vi_userindex = recordloc;
	user_hash.vi_link = 0;
	fput(&user_hash, 16, fp);	

	tableloc = hash_index(username);
	rewind(fp);	

	if (hashtable[tableloc] == 0) {  /* no collision */
		hashtable[tableloc] = indexloc;
		fput(hashtable, 512, fp);	/* rewrite hash table */
	}
	else { /* collision */
		int		curloc;

		curloc = 0;
		hash_read(fp, &user_hash, hashtable[tableloc], &curloc);
		while (user_hash.vi_link != 0) 
			hash_read(fp, &user_hash, user_hash.vi_link, &curloc);			
		fiseek(fp, -16); /* backup 1 record */
		user_hash.vi_link = indexloc;		
		fput(&user_hash, 16, fp);
	}
	
	fclose(fp);
	return(recordloc);
	
}


unsigned valid_delete(userindex)
/* Procedure valid_delete - delete an entry from the valid file and
zero out its valindx entry. */
unsigned	userindex;
{
	struct	va_user_record	user_record;
	struct	vi_user_hash	user_hash, user_hash2;
	FILE	*fp;
	int 	prevlink, curloc, hashindex, index;
	int    	hashtable[256];

	
	if (valid_read(userindex, &user_record) == 0)
		return(0);
	
	hashindex = hash_index(user_record.va_username);
	index = user_record.va_userindex;
                                
	zero(&user_record, sizeof(user_record));
	if (valid_write(userindex, &user_record) == 0)
		return(0);

	if ((fp = fopen("/val/valindx","rwv")) == 0)
		return(0);
	
	if (fget(hashtable, 512, fp) != 512)
		error("valid:fatal - missing hash record in valindx!\n");
                  
	if (hashtable[hashindex] == 0)
		error("valid:fatal - hash entry is zero.\n");

	curloc = 512;
	if (hashtable[hashindex] == index) {
		hash_read(fp, &user_hash, index, &curloc);
		hashtable[hashindex] = user_hash.vi_link;
		rewind(fp);
		fput(hashtable, 512, fp);
		curloc = 512;
	}
	else {
		hash_read(fp, &user_hash, hashtable[hashindex], &curloc);
		prevlink = hashtable[hashindex];
		while (user_hash.vi_link != index) {
			prevlink = user_hash.vi_link;
			hash_read(fp, &user_hash, prevlink, &curloc);
		}
		hash_read(fp, &user_hash2, index, &curloc);	
		user_hash.vi_link = user_hash2.vi_link;
		hash_write(fp, &user_hash, prevlink, &curloc);
	}
	user_hash.vi_username[0] = '\0';
	user_hash.vi_userindex = 0;
	user_hash.vi_link = 0;
	hash_write(fp, &user_hash, index, &curloc);
	fclose(fp);
	return(userindex);  /* success */
}
