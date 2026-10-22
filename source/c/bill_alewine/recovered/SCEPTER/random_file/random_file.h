/*		random_file.h - random_file definitions.
		84/04/14.	 A. E. Klietz
*/


#define		N_CHECK			0xD1A5		/* header of node */
#define		MAXFILES		5 			/* Max number of open random files */
#define		HASHSIZE		61			/* Size of hash table (prime #) */
#define		CACHESIZE		17			/* Size of cache table (prime #) */
#define		SIZE_CHUNK		512			/* Size of chunk to increase file space */
#define		NUM_CHUNKS		32			/* Number of chunks to increase each
										   time file space runs out */

/* Definition for a node in a random_file file */

struct n_node {
	unsigned_word	n_checkword;		/* start of new node */
	word			n_allocflag;		/* < 0 if allocated. */
	unsigned_word	n_index;			/* index of node */
	unsigned_word	n_size;				/* size of node. */
	unsigned_word	n_data_size;		/* size of data in node */
	char			n_data[0];			/* start of data for node. */
};	

#define 	NULL_NODE	((struct n_node *) 0)
#define		NODE_SIZE	sizeof(struct n_node)

#define		N_MAXSIZE   16384  /* Maximum size of node = SIZE_CHUNK * NUM_CHUNKS */
#define		N_MINSIZE	500	   /* Minimum number of unused bytes before the node
								  gets split apart. */


/* Definition of a node pointer */

struct np_node_pointer {
	struct np_node_pointer	*np_next;	/* link to next node */
	longword				np_offset; /* offset of node into file */
	union {
		unsigned_word		np_sizenode;  /* size of free node */
		unsigned_word		np_index;	  /* index of allocated node */
	} np_data;
};

#define	NULL_NODE_POINTER  ((struct np_node_pointer *) 0)



/* Definition of a cache pointer */

struct cp_cache_pointer {
	struct cp_cache_pointer	*cp_next;	/* link to next node */
	unsigned_word			cp_index;	/* index of cached node */
	unsigned_word			cp_sizedata; /* size of data in node */
	char					*cp_data;	/* data inside cached node */
};

#define NULL_CACHE_POINTER ((struct cp_cache_pointer *) 0)



/* Definition of a random file descriptor */

struct rf_random_file {
	FILE					*rf_fp;		/* File (0 if closed) */
	struct	np_node_pointer	*rf_AV;		/* Head of available space list */		
	struct	np_node_pointer *rf_hashtbl[HASHSIZE]; /* table of allocated nodes */
	char					rf_name[BUFLEN]; /* name of file */
	struct	np_node_pointer	*rf_allocp;	/* current position of end-round ptr */
	unsigned_word			rf_maxcachesize; /* limit of data in cache */
	unsigned_word			rf_cachesize;	/* current size of cache */
	struct cp_cache_pointer *rf_cachetbl[CACHESIZE]; /* cache table */
	word					rf_cache_nextremove; /* end-around cache removal ptr */
};

#define	NULL_RANDOM_FILE ((struct rf_random_file *) 0)



/* Definition of the root block */

struct rb_root_block {
	unsigned_word			rb_corrupt;		/* if nonzero, then file is corrupt */
	unsigned_word			rb_checkword;	/* Identify file as a random file */
	unsigned_word			rb_maxcachesize;	/* size of cache in bytes */
	longword				rb_filesize;	/* size of file in bytes */
	struct n_node		  	rb_AVnode;		/* Head of available node list */
	byte					rb_dummy[480 - sizeof(rb_corrupt) - 
								sizeof(rb_checkword) - sizeof(rb_filesize)
								- sizeof(rb_maxcachesize) - sizeof(rb_AVnode)];
};



extern	struct	rf_random_file	filelist[MAXFILES];
/* Master file list of random files. */


/*
		random_read(fp, word index, char *data, word maxsize)

			read data from file fp at index.  A maximum of
maxsize words are returned.  The number of words read is returned. 



		random_write(fp, word index, char *data, word size)

			write data to file fp at index.  The number of words
written is returned.



   fp = random_open(filename)

			Open a random file.  Return zero if unable to open			
file.  



   fp = random_create(filename, initsize, cachesize)

			Create and open file with an initial length of 'initsize'
bytes.  Size must be at least 1024.  'cachesize' is size of cache
(up to 16384 bytes).  Zero is returned if file cannot be created. 



		random_close(fp)
			Close the given random file



		random_compact(fp)

			Request a compaction of file fp.  This could take 
over a minute for large files.
*/
