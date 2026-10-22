/*		Random_msgs.h - Random file message definitions
		84/04/28.	A. E. Klietz.

(hint for AEK: use varying length strings for object/monster names

changed scepter constants  - max objs per room - 25
							 rooms per cluster -  5)

*/

#define		MAX_XFER_SIZE	256  /* size of smallest xfer w/o size reply */
#define		PARAM_SIZE		 20	 /* size of parameters other than databuf */

enum ra_msg_type {RA_READMSG, RA_READSIZEMSG, RA_WRITEMSG, RA_WRITESIZEMSG,
	RA_PROCEEDMSG, RA_OPENMSG, RA_CREATEMSG, RA_CLOSEMSG, RA_COMPACTMSG};


struct  ra_generic_msg {
	word	ra_checkword;       /* N_CHECK checkword */
	word	ra_msgnum;          /* message number (read, write, etc) */
	byte	ra_dummy[PARAM_SIZE]; /* other misc parameters */
	char	ra_databuf[MAX_XFER_SIZE];
};

struct ra_proceed {         	/* Proceed with data transfer */
	word	ra_checkword;
	word	ra_msgnum;
};

struct	ra_readmsg {
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_index;       /* Index of record to read */
	word	ra_fp;          /* File pointer of file to read */
	word	ra_maxsize;     /* Max size of record to read */
};

struct	ra_readsize_reply { /* Size of upcoming read reply */
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_readsize;
};

struct	ra_readmsg_reply {
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_readsize;
	char	ra_read_data[0];
};

struct	ra_writesizemsg { /* Size of upcoming write */
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_writesize;
};

struct	ra_writemsg {
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_index;		/* Index of record to write */
	word	ra_fp;          /* file pointer of file to write */ 
	word	ra_data_writesize;   /* Size of data to write (up to 512) */
	word	ra_write_data[0];
};

struct	ra_writemsg_reply {
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_writesize; /* Size of data actually written. 0 if error. */
};

struct ra_callcomplete {
	word	ra_complete_code;	/* returned value from open/close/compact/create */
};

struct	ra_openmsg {
	word	ra_checkword;
	word	ra_msgnum;
	char	ra_filename[BUFLEN];
};

struct	ra_createmsg {
	word	ra_checkword;
	word	ra_msgnum;
	char	ra_filename[BUFLEN];
	word	ra_initsize;
	word	ra_cachesize;
};

struct	ra_closemsg {
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_cdummy;
	word	ra_fp;
};

struct	ra_compactmsg {
	word	ra_checkword;
	word	ra_msgnum;
	word	ra_cdummy;
	word	ra_fp;
};

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
