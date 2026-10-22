/*		random_read - perform read of random file.
		84/04/15.     A. E. Klietz.
*/

#include "/std/def.h"
#include "random_file.h"
#include "random_msgs.h"

word random_read(fp, index, data, maxsize)
/* Read record from random file at the given index.  Up to 'maxsize'
bytes are read.  The actual number of bytes read is returned. 
KLUDGE:	
	'data' is allocated from free memory to fit the correct size
	And top few words are reserved for ra_readmsg_reply information */
	
unsigned_word	fp;
unsigned_word   index;
char			**data; /* Pointer to character string pointer */
unsigned_word	maxsize;
{
	register	struct		rf_random_file		*filedesc;
	register	struct		np_node_pointer		*node_pointer;
	struct		rf_random_file	*get_rf_index();
	word		hash, hash_index(), finalsize;
	boolean		read_node();
	struct		n_node				node;

	if ((filedesc = get_rf_index(fp)) == NULL_RANDOM_FILE)
		/* If no such fp found */
		return(0);  /* no bytes read. */

	hash = hash_index(index, HASHSIZE);
	/* Generate hash bucket for index */

	node_pointer = filedesc->rf_hashtbl[hash];  /* head node in bucket */

	while (node_pointer != NULL_NODE_POINTER && 
				node_pointer->np_data.np_index != index) 
		/* scan links in bucket for a match */
		node_pointer = node_pointer->np_next;

	if (node_pointer == NULL_NODE_POINTER) /* if no such index */
		return(0);  

	if ((finalsize = read_cache(filedesc, index, maxsize, data)) != 0) 
		return(finalsize);  /* if found in cache */

	if (!read_node(filedesc, node_pointer->np_offset, TRUE, &node))
		return(0);  /* if bad node read */

	if (node.n_allocflag >= 0 || /* if not allocated */
		node.n_index != index || /* if index mismatch */
		node.n_checkword != N_CHECK) { /* if bad check bits */
		errlog("warn:random_read - bad node in %s at %d (index %d).\n",
			filedesc->rf_name, (word) node_pointer->np_offset, index);
		return(0);
	}
                          
	*data = alloc(node.n_data_size + sizeof(struct ra_readmsg_reply));
	if ((finalsize = fget(*data + sizeof(struct ra_readmsg_reply),
			node.n_data_size, filedesc->rf_fp))
		!= node.n_data_size)
		errlog("warn:random_read - truncated read. in %s at %d (index %d).\n",
			filedesc->rf_name, (word) node_pointer->np_offset, index);
	else {
		add_cache(filedesc, index, finalsize, *data + sizeof(struct ra_readmsg_reply));
		if (node.n_data_size > maxsize) { /* if too large */
			free(*data);
			*data = realloc(*data, maxsize + sizeof(struct ra_readmsg_reply));
			finalsize = maxsize;
		}
	}
	return(finalsize);
}


struct	rf_random_file *get_rf_index(fp)
/* Return a pointer to file descriptor table based on the fp */
unsigned_word	fp;
{
	if (fp <= 0 || fp > MAXFILES)
	/* if not active file */
		return(NULL_RANDOM_FILE);
	else {
		if (filelist[fp-1].rf_fp == 0)
			return(NULL_RANDOM_FILE);
		else
			return(&filelist[fp-1]);
	}
}


word hash_index(index, hashsize)
/* Compress an arbitrary index down into the range [0..HASHSIZE-1]
   hashsize should be a prime number for best results. */
unsigned_word	index, hashsize;
{
	return(abs(index % hashsize));
}
