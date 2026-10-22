/*		random_close - Close a random file.
		84/04/22.     A. E. Klietz.
*/

#include "/std/def.h"
#include "random_file.h"



random_close(fp)
word	fp;
{
struct	rf_random_file	*filedesc;

	if ((filedesc = get_rf_index(fp)) == NULL_RANDOM_FILE) 
		return; /* if bad index */

	random_fpclose(filedesc);
}



random_fpclose(filedesc)
/* close a random file, free all its links, and zero the entry */
struct	rf_random_file	*filedesc;
{
	struct		np_node_pointer	*node_pointer, *tmp_node_pointer;
	struct		cp_cache_pointer	*tmp_cache_pointer;

	fclose(filedesc->rf_fp);
	
	node_pointer = filedesc->rf_AV;

	/* free memory of free-node list */
	while (node_pointer->np_next != filedesc->rf_AV) {
		tmp_node_pointer = node_pointer->np_next;		
		node_pointer->np_next = node_pointer->np_next->np_next;	
		free(tmp_node_pointer);
	}
	free(filedesc->rf_AV);

{   /* free memory of hash table lists */
	register i;

	for (i=0; i < HASHSIZE; ++i) 
		while (filedesc->rf_hashtbl[i] != NULL_NODE_POINTER) {
			tmp_node_pointer = filedesc->rf_hashtbl[i];
			filedesc->rf_hashtbl[i] = filedesc->rf_hashtbl[i]->np_next;
			free(tmp_node_pointer);
		}

	for (i=0; i < CACHESIZE; ++i)
		while(filedesc->rf_cachetbl[i] != NULL_CACHE_POINTER) {
			tmp_cache_pointer = filedesc->rf_cachetbl[i];
			filedesc->rf_cachetbl[i] = filedesc->rf_cachetbl[i]->cp_next;
			free(tmp_cache_pointer->cp_data);
			free(tmp_cache_pointer);
		}
}

	zero (filedesc, sizeof(struct rf_random_file));
}
