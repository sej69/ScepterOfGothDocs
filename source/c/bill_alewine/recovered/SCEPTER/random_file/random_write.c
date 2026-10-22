/*		random_write - perform write of random file.
		84/04/15.     A. E. Klietz.
 
        CopyRight (c) 1984, Alan E. Klietz.   All Rights Reserved.
*/

#include "/std/def.h"
#include "random_file.h"

word random_write(fp, index, data, size)
/* Write a record to a random file at the given index.  'size'
bytes are written.  The actual number of bytes written is returned. */
unsigned_word	fp;
unsigned_word	index;
char			*data;
unsigned_word	size;
{
	register	struct	rf_random_file		*filedesc;
	struct		rf_random_file		*get_rf_index();
	word		hash, finalsize, 	hash_index();
	register	struct		np_node_pointer		*node_pointer;
	struct		np_node_pointer		*alloc_node(), *realloc_node();
	struct		n_node				node;
	boolean		read_node(), write_node();

	if ((filedesc = get_rf_index(fp)) == NULL_RANDOM_FILE)
		/* If no such fp found */
		return(0);  /* no bytes read. */

	if (size < 0)
		size == 0;
	else
		if (size + NODE_SIZE > N_MAXSIZE) /* if node too large */
			size = N_MAXSIZE - NODE_SIZE;

	hash = hash_index(index, HASHSIZE);
	/* Generate hash bucket for index */

	node_pointer = filedesc->rf_hashtbl[hash];  /* head node in bucket */

	while (node_pointer != NULL_NODE_POINTER && 
				node_pointer->np_data.np_index != index) 
		/* scan links in bucket for a match */
		node_pointer = node_pointer->np_next;

	if (node_pointer == NULL_NODE_POINTER && size > 0) { /* if no such index */
		if ((node_pointer = alloc_node(filedesc, index,
				size + NODE_SIZE, &node.n_size)) == NULL_NODE_POINTER)
			/* if unable to allocate new node */
			return(0);
	}
	else {
		remove_cache(filedesc, index);  /* remove from cache */
		if (size > 0) { /* if not to delete */
			if (!read_node(filedesc, node_pointer->np_offset, TRUE, &node))
				return(0); /* if node read */
			if (size + NODE_SIZE > node.n_size || 
					node.n_size > size + NODE_SIZE + N_MINSIZE)
				/* if data too big or shrunk enough to spawn empty node */
			   	if ((node_pointer = realloc_node(filedesc, index, size + 
                   NODE_SIZE, node_pointer, &node.n_size)) == NULL_NODE_POINTER)
					/* if unable to allocate new node */
					return(0);
		}
		else /* node to be deleted */
			if (node_pointer != NULL_NODE_POINTER) { /* if node exists */
				free_node(filedesc, index, node_pointer); /* release node */
                rewind(filedesc->rf_fp);   /* KLUDGE - flush output to disk */
				return(0);
			}	
			else
				return(0); /* tried to free nonexistant node */
	}

	node.n_checkword = N_CHECK;  /* Construct and write a node header */
	node.n_allocflag = -1;
	node.n_index = index;	
	node.n_data_size = size;

	if (!write_node(filedesc, node_pointer->np_offset, TRUE, &node))
		/* if bad write */
		return(0);

	if ((finalsize = fput(data,	size, filedesc->rf_fp)) != size) {
		 /* if fput failed */
		errlog("warn:random_write - write failed in %s at %d (index %d).\n",
		filedesc->rf_name, (word) node_pointer->np_offset, index);
		return(0);
	}
	rewind(filedesc->rf_fp); /* KLUDGE - flush data to disk - KLUDGE */
	return(finalsize);
}



struct np_node_pointer *alloc_node(filedesc, index, size, actual_size)
/* Search the AV list for a node of at least 'size' bytes using an
end-around first fit algorithm.  Grow file if not found
otherwise shrink the node in the AV list
(or unlink it if its too small) and alloc and insert the new node in the
filedesc->rf_hashtbl[].   Return actual_size. */

struct	rf_random_file	*filedesc;
unsigned_word	index;
unsigned_word	size;
word	*actual_size;
{
	register	struct	np_node_pointer *new_node_ptr, *allocp, *alloc_trailer;
	struct		np_node_pointer		*old_allocp;
	struct	n_node	node;
	boolean	read_node(), write_node();
	word	hash;
	longword	offset;
	
	old_allocp = alloc_trailer = allocp = filedesc->rf_allocp;

	for (allocp = allocp->np_next; ; 
			alloc_trailer = allocp, allocp = allocp->np_next) {
		if (allocp->np_data.np_sizenode >= size) { /* if node fits */
			/* create node and insert in hash table */
			new_node_ptr = alloc(sizeof(struct np_node_pointer));
			copy(new_node_ptr, allocp, sizeof(struct np_node_pointer));
			new_node_ptr->np_data.np_index = index;
			hash = hash_index(index, HASHSIZE);
			new_node_ptr->np_next = filedesc->rf_hashtbl[hash];
			filedesc->rf_hashtbl[hash] = new_node_ptr;

			if (allocp->np_data.np_sizenode > size + N_MINSIZE) {	
				/* if node can be split */
				new_node_ptr->np_offset += allocp->np_data.np_sizenode - size;
				*actual_size = size;
#ifdef PARANOID
				if (!read_node(filedesc, allocp->np_offset, TRUE, &node))
					return(NULL_NODE_POINTER); /* if bad read */
#else
				node.n_checkword = N_CHECK;
				node.n_index = index;
#endif
				allocp->np_data.np_sizenode -= size;  /* shrink size */
				node.n_size = allocp->np_data.np_sizenode;			
				node.n_allocflag = 0;
				node.n_data_size = 0;
				if (!write_node(filedesc, allocp->np_offset, TRUE, &node))
					return(NULL_NODE_POINTER); /* if bad write */
			}					
			else { /* delete node from free list */ 
				*actual_size = allocp->np_data.np_sizenode;
				alloc_trailer->np_next = allocp->np_next;
				free(allocp);
				allocp = alloc_trailer->np_next;
			}
			break;
		}
		if (allocp == old_allocp) { /* if no space found */
			filedesc->rf_allocp = allocp; /* update allocp */
			more_filespace(filedesc);  /* get more file space */
			allocp = old_allocp = alloc_trailer = filedesc->rf_allocp;
		}
	}		
	filedesc->rf_allocp = allocp;  /* update alloc pointer */
	return(new_node_ptr);
}
		




more_filespace(filedesc)
/* Get more file space. */
struct  rf_random_file  *filedesc;
{
	word	i;
	char		*zeros;
	register	struct	np_node_pointer	*new_node_ptr;
	struct	rb_root_block root_block;
	struct	n_node	node;
	boolean	write_node();

	rewind(filedesc->rf_fp);
	if (fget(&root_block, sizeof(root_block), filedesc->rf_fp)
		!= sizeof(root_block)) 
		crash("fatal:more_filespace - unable to read root block!\n");

	/* insert new space in hash table and then free_node() it. */
	new_node_ptr = alloc(sizeof(struct np_node_pointer));
	i = hash_index(0, HASHSIZE);
	new_node_ptr->np_next = filedesc->rf_hashtbl[i];
	filedesc->rf_hashtbl[i] = new_node_ptr;
	new_node_ptr->np_offset = root_block.rb_filesize;	
	new_node_ptr->np_data.np_index = 0;

	root_block.rb_corrupt = 1; /* flag file as corrupt */
	root_block.rb_filesize += SIZE_CHUNK * NUM_CHUNKS;
	rewind(filedesc->rf_fp);
	fput(&root_block, sizeof(root_block), filedesc->rf_fp);
	fseek(filedesc->rf_fp, new_node_ptr->np_offset, 0);
	zeros = alloc(SIZE_CHUNK); /* divide into chunks */		
	for (i=0; i < NUM_CHUNKS; ++i)	
		if (fput(zeros, SIZE_CHUNK, filedesc->rf_fp) != SIZE_CHUNK)
			crash("fatal:more_filespace - fput error while increasing size of %s.\n",
				filedesc->rf_name);
	free(zeros);
	
	node.n_checkword = N_CHECK;
	node.n_allocflag = 0;
	node.n_index = 0;
	node.n_size = NUM_CHUNKS * SIZE_CHUNK;
	node.n_data_size = 0;
	write_node(filedesc, new_node_ptr->np_offset, TRUE, &node);

	fclose(filedesc->rf_fp);  /* close and reopen file in order to update
								 directory so file will not be corrupted
									 if the system should crash. */

	if ((filedesc->rf_fp = fopen(filedesc->rf_name, "rwv")) == 0)
		/* if cannot reopen file */
		crash("alloc:fatal - cannot reopen file %s!\n",
			filedesc->rf_name);							
	i = 0; /* mark file as restored */
	fput(&i, sizeof(word), filedesc->rf_fp);

    free_node(filedesc, 0, new_node_ptr);  /* link node into free list */
}



free_node(filedesc, index, node_ptr)
struct rf_random_file	*filedesc;
unsigned_word			index;
struct np_node_pointer	*node_ptr;
{
	word	hash;
	register	struct	np_node_pointer *temp_node_ptr;
	struct	n_node	node, othernode;
	longword	offset;
	boolean		remove_allocnode(), read_node(), write_node();

	if (index != node_ptr->np_data.np_index) {
		errlog("warn:free_node - index mismatch. %d - %d.\n", index,
			node_ptr->np_data.np_index);
		return;
	}

	if (remove_allocnode(filedesc, index, node_ptr) == FALSE)
		return;

	/* insert in free list */
	node_ptr->np_next = filedesc->rf_AV->np_next;
	filedesc->rf_AV->np_next = node_ptr;
	if (!read_node(filedesc, node_ptr->np_offset, TRUE, &node))
		return;
	node.n_data_size = 0;
	node.n_allocflag = 0;
	node_ptr->np_data.np_sizenode = node.n_size;
#ifndef PARANOID
	fseek(filedesc->rf_fp, node.n_size - NODE_SIZE, 1);
#endif

	for (;;) { /* check adjacent node and merge if possible */
		
#ifndef PARANOID
		if (fget(&othernode, NODE_SIZE, filedesc->rf_fp) != NODE_SIZE) 
#else
		if (!read_node(filedesc, node_ptr->np_offset + node.n_size,
				FALSE, &othernode)) /* if past  end of file */
#endif
			break;
			
		if (othernode.n_allocflag < 0) /* if allocated */
			break; /* stop merging */	

		node.n_size += othernode.n_size; /* merge */
#ifndef PARANOID
		fseek(filedesc->rf_fp, othernode.n_size - NODE_SIZE, 1);
#endif
	}
	
	if (!write_node(filedesc, node_ptr->np_offset, TRUE, &node))
		return;
}



struct np_node_pointer *realloc_node(filedesc, index, size, node_pointer, actual_size)
/* reallocate a node to a new size. */
struct rf_random_file *filedesc;
unsigned_word	index;
unsigned_word			size;
struct np_node_pointer *node_pointer;
unsigned_word			*actual_size;
{
	struct	np_node_pointer	*alloc_node();

	free_node(filedesc, index, node_pointer);
	return(alloc_node(filedesc, index, size, actual_size));
}



boolean read_node(filedesc, offset, err_flag, node)
/* read node at absolute offset.
TRUE is returned if the read is successful */
struct 	rf_random_file *filedesc;
longword offset;
boolean	err_flag;
struct	n_node	*node;
{
	if (fseek(filedesc->rf_fp, offset, 0) < 0) {
	/* if seek failed */
		if (err_flag)
			errlog("warn:read_node - seek failed for %s to %d.\n",
				filedesc->rf_name, (word) offset);
		return(FALSE);
	}
	if (fget(node, NODE_SIZE, filedesc->rf_fp) != NODE_SIZE) {
		if (err_flag)
			errlog("warn:read_node - node fget failed in %s (offset %d)\n",
				filedesc->rf_name, (word) offset);	
		return(FALSE);
	}
	if (node->n_checkword != N_CHECK) {
		errlog("warn:read_node - bad checkword at %d in %s.\n",
			(word) offset, filedesc->rf_name);
		return(FALSE);
	}
	return(TRUE);
}


boolean write_node(filedesc, offset, err_flag, node)
/* write node at absolute offset.  
TRUE is returned if the write is successful */
struct 	rf_random_file *filedesc;
longword	offset;	
boolean	err_flag;
struct	n_node	*node;
{

	if (fseek(filedesc->rf_fp, offset, 0) < 0) {
	/* if seek failed */
        if (err_flag)
			errlog("warn:write_node - seek failed for %s to %d.\n",
				filedesc->rf_name, (word) offset);
		return(FALSE);
	}

	if (fput(node, NODE_SIZE, filedesc->rf_fp) != NODE_SIZE) {
		if (err_flag)
			errlog("warn:write_node - node fget failed in %s (offset %d)\n",
				filedesc->rf_name, (word) offset);	
		return(FALSE);
	}
	return(TRUE);
}


boolean remove_allocnode(filedesc, index, node_ptr)
/* remove the given node from the allocated node hash table. */
struct	rf_random_file	*filedesc;
unsigned_word	index;
struct	np_node_pointer	*node_ptr;
{
	word	hash;
	register	struct	np_node_pointer	*temp_node_ptr;

	/* Remove node from hash table */
	hash = hash_index(index, HASHSIZE);		
	temp_node_ptr = filedesc->rf_hashtbl[hash];
	if (temp_node_ptr == NULL_NODE_POINTER) {
		errlog("warn:free_node - attempted to free empty hash bucket at %d\n",
			index);
		return(FALSE);
	}

	if (temp_node_ptr == node_ptr)
		filedesc->rf_hashtbl[hash] = filedesc->rf_hashtbl[hash]->np_next;
	else {	
		while (temp_node_ptr->np_next != NULL_NODE_POINTER
				&& temp_node_ptr->np_next != node_ptr)
				temp_node_ptr = temp_node_ptr->np_next;

		if (temp_node_ptr->np_next == NULL_NODE_POINTER) {
			errlog("warn:free_node - unable to delete index %d.\n", index);
			return(FALSE);
		}
		temp_node_ptr->np_next = node_ptr->np_next;
	}
	return(TRUE);
}
