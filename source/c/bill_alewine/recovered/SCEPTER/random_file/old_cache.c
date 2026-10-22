/*		random_cache - routines for reading/writing the
					   in-memory cache.
		84/04/22. 	A. E. Klietz.
*/

#include "/std/def.h"
#include "random_file.h"
#include "random_msgs.h"

word read_cache(filedesc, index, maxsize, data)
/* check the cache and read in the data if possible.
KLUDGE:
   'data' is allocated to necessary size 
	Reserve top few words for ra_readmsg_reply information */

struct rf_random_file	*filedesc;
unsigned_word			index;
unsigned_word			maxsize;
char					**data; /* pointer to data pointer */
{
	register	struct	cp_cache_pointer	*cache_pointer;
    word	hash;
	word	size;

	if (filedesc->rf_maxcachesize == 0)

		return(0); /* if no cache */

	hash = abs(index % CACHESIZE);
    cache_pointer = filedesc->rf_cachetbl[hash];
	while (cache_pointer != NULL_CACHE_POINTER &&
			cache_pointer->cp_index != index)
		cache_pointer = cache_pointer->cp_next;

	if (cache_pointer == NULL_CACHE_POINTER) /* if not found */
		return(0);

	if ((size = min(maxsize, cache_pointer->cp_sizedata)) == 0)
		return(0); /* if no data */

	*data = alloc(size + sizeof(struct ra_readmsg_reply));
	copy(*data + sizeof(struct ra_readmsg_reply), cache_pointer->cp_data, size);
	return(size);
}
	


remove_cache(filedesc, index)
/* remove the given item from the cache */
struct	rf_random_file	*filedesc;
unsigned_word	index;
{
	register	struct	cp_cache_pointer	*cache_pointer, *tmp_cache_pointer;
    word	hash;
	word	size;

	if (filedesc->rf_maxcachesize == 0)
		return; /* if no cache */

	hash = abs(index % CACHESIZE);
    tmp_cache_pointer = filedesc->rf_cachetbl[hash];
	if (tmp_cache_pointer == NULL_CACHE_POINTER) 
		return;

	if (tmp_cache_pointer->cp_index == index) {	
		cache_pointer = tmp_cache_pointer;
		filedesc->rf_cachetbl[hash] = cache_pointer->cp_next;
	}
	else {	
		while (tmp_cache_pointer->cp_next != NULL_CACHE_POINTER &&
					tmp_cache_pointer->cp_next->cp_index != index)
				tmp_cache_pointer = tmp_cache_pointer->cp_next;

		if (tmp_cache_pointer->cp_next == NULL_CACHE_POINTER) 
			return;

		cache_pointer = tmp_cache_pointer->cp_next;
		tmp_cache_pointer->cp_next = cache_pointer->cp_next;
	}
	filedesc->rf_cachesize -= cache_pointer->cp_sizedata;
	free(cache_pointer->cp_data);
	free(cache_pointer);
}



add_cache(filedesc, index, size, data)
/* Add a new node to the cache (if it fits).  Delete an old item using
   an end-around last-in-bucket search if needed to make room in the cache */
struct rf_random_file	*filedesc;
unsigned_word			index;
unsigned_word			size;
char					*data;
{
	struct	cp_cache_pointer	*cache_pointer, *tmp_cache_pointer;
    word	hash;

	if (filedesc->rf_maxcachesize == 0 || size == 0)
		return;	/* if no cache, or no data */

	if (size <= 0 || size > filedesc->rf_maxcachesize / 4)
		return; /* if too big for cache */


	while (size + filedesc->rf_cachesize > filedesc->rf_maxcachesize) {
		/* go end-around and remove the bottom node from the bucket */
 		register	i;

		i = filedesc->rf_cache_nextremove;
		while (filedesc->rf_cachetbl[i] == NULL_CACHE_POINTER)
			i = (i == 0 ? CACHESIZE - 1 : i-1);
		tmp_cache_pointer = filedesc->rf_cachetbl[i];
		if (tmp_cache_pointer->cp_next == NULL_CACHE_POINTER) {
			cache_pointer = tmp_cache_pointer;
			filedesc->rf_cachetbl[i] = NULL_CACHE_POINTER;
		}
		else {
			while(tmp_cache_pointer->cp_next->cp_next != NULL_CACHE_POINTER)
				tmp_cache_pointer = tmp_cache_pointer->cp_next;

			cache_pointer = tmp_cache_pointer->cp_next;
			tmp_cache_pointer->cp_next = NULL_CACHE_POINTER;		
		}
		filedesc->rf_cache_nextremove = (i == 0 ? CACHESIZE - 1 : i-1);
		filedesc->rf_cachesize -= cache_pointer->cp_sizedata;
		free(cache_pointer->cp_data);
		free(cache_pointer);
    }

    /* now add new cache entry to table */
	
	cache_pointer = alloc(sizeof(struct cp_cache_pointer));
	cache_pointer->cp_index = index;
	cache_pointer->cp_sizedata = size;
	cache_pointer->cp_data = alloc(size);
	copy(cache_pointer->cp_data, data, size);
	hash = abs(index % CACHESIZE);
	cache_pointer->cp_next = filedesc->rf_cachetbl[hash];	
	filedesc->rf_cachetbl[hash] = cache_pointer;
	filedesc->rf_cachesize += size;
}
