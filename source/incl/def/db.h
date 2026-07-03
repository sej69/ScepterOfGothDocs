/*	db.h - database definitions.
	84/12/20.  A. E. Klietz.


	int      db_open(name, mode);

	  	 db_close(fd);

	unsigned db_read(fd, id, buffer, len);
		 unsigned len;

	unsigned db_write(fd, id, buffer, len);
		 unsigned len;

		 db_open_key(fd);

	char	 *db_key(fd);

		 db_close_key(fd);
*/

#define	 NAMELEN   16
#define  DB_TABLELEN  5
#define  DB_CHECKWORD 0xA5C9

extern  long lseek();
extern  long tell();

struct	ht_hashtable {
	union {
		char	ht_name[NAMELEN];
		long	ht_long[NAMELEN / sizeof(long)];
	} ht;
	off_t	 ht_offset;
	unsigned ht_length;  /* zero if empty, DB_EMPTY if dirtied */
};

struct	db_dbtable {
	short		db_checkword;
	short		db_fd;
	short		db_hashfd;
	short		db_lockfd;
	short		db_wflg;
	short		db_keyfd;
	int		db_slot_count;   /* For db_key */
	int		db_bucket_count; /* For db_key */
	int		db_num_hashbuckets;
	char		db_filename[BUFLEN];
};


struct	hd_header {
	short		hd_checkword;
	char		hd_name[NAMELEN];
	unsigned	hd_real_length;
	unsigned	hd_length;
};

#define DB_EMPTY	((unsigned) ~0)
#define DB_MAXRECSIZE   (((unsigned) ~0)-1)
