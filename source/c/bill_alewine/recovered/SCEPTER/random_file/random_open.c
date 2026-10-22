/*		random_open - Open a random file.
		84/04/20.     A. E. Klietz.
*/

#include "/std/def.h"
#include "random_file.h"


struct	rf_random_file	filelist[MAXFILES];

word random_open(filename)
char	*filename;
{
	register	i;

	for (i=0; i < MAXFILES; ++i) 
		if (filelist[i].rf_fp == 0)
			break;

	if (i >= MAXFILES) {
		errlog("warn:random_open - too many open files.\n");
		return(0);
	}

    if (random_fpopen(&filelist[i], filename) == 0)
		return(0);
	else
		return(i+1);
}


word random_fpopen(filedesc, filename)
/* Open random file.  Return nonzero if successful */
struct	rf_random_file	*filedesc;
char	*filename;
{
	FILE		*fp;
	struct		rb_root_block	root_block;
	struct		n_node			node;
	longword	filesize;
	word	hash, i;
	register	struct		np_node_pointer	*node_pointer;

	zero (filedesc, sizeof(struct rf_random_file));

	if ((fp = fopen(filename, "rwv")) == 0) /* if cannot open */		
		return(0);

	if (fget(&root_block, sizeof(root_block), fp) != sizeof(root_block)) {
		errlog("warn:random_open - unable to read root block of %s.\n",
			filename);
		fclose(fp);
		return(0);
	}

	if (root_block.rb_checkword != N_CHECK) {
		errlog("warn:random_open - bad checkword on %s.\n", filename);
		fclose(fp);
		return(0);
	}

	if (root_block.rb_corrupt != 0) {
		errlog("warn:random_open - %s is corrupt!\n",
			filename);
		fclose(fp);
		return(0);
	}

	filesize = sizeof(root_block);

	/* Set up AV node */
	node_pointer = alloc(sizeof(struct np_node_pointer));
	filedesc->rf_AV = node_pointer;
	node_pointer->np_next = node_pointer;
	node_pointer->np_offset = (word) &root_block.rb_AVnode - 
		(word) &root_block;
	node_pointer->np_data.np_sizenode = 0;

	filedesc->rf_fp = fp;
	filedesc->rf_maxcachesize = root_block.rb_maxcachesize;
	strcpy(filedesc->rf_name, filename);
	while (filesize < root_block.rb_filesize) {
		if (fget(&node, NODE_SIZE, filedesc->rf_fp) != NODE_SIZE) {
			errlog("warn:random_open - bad node read at %04x in %s.\n",
				(word) filesize, filename);
			fclose(fp);
			filedesc->rf_fp = 0;
			return(0);
		}		
		if (node.n_checkword != N_CHECK) { 
			errlog("warn:random_open - bad checkword at %04x in %s.\n",
				(word) filesize, filename);
			fclose(fp);
				filedesc->rf_fp = 0;
				return(0);
		}

		
		node_pointer = alloc(sizeof(struct np_node_pointer));				
		node_pointer->np_offset = filesize;

		if (node.n_size < sizeof(node) || node.n_size > N_MAXSIZE) {
			errlog("warn:random_open - bad node size of %d in %s at %04x.\n",
				node.n_size, filename, (word) filesize);
			fclose(fp);
			filedesc->rf_fp = 0;
			return(0);
		}

		if (node.n_allocflag < 0) { /* if allocated */
			/* insert in hash table */
		    hash = hash_index(node.n_index, HASHSIZE);
			node_pointer->np_next = filedesc->rf_hashtbl[hash];
			filedesc->rf_hashtbl[hash] = node_pointer;
			node_pointer->np_data.np_index = node.n_index;
		}
		else { /* insert in link table */
			node_pointer->np_next = filedesc->rf_AV->np_next;	
			filedesc->rf_AV->np_next = node_pointer;
			node_pointer->np_data.np_sizenode = node.n_size;
		};
	
		fseek(fp, (longword) (node.n_size - sizeof(node)), 1);
		filesize += node.n_size;
	}


	if (fget(&i, 1, fp) > 0) { /* if more data */
		errlog("warn:random_open - more data beyond end of file %s.\n",
			filename);
		fclose(fp);
		filedesc->rf_fp = 0;
		return(0);
	}

	filedesc->rf_allocp = filedesc->rf_AV;
	rewind(fp);   /* KLUDGE - flush data to disk???  KLUDGE */
	return(fp);	
}
