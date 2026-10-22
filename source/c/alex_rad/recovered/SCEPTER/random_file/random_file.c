/*		random_file - Random file manager.
		84/05/04.	  A. E. Klietz.

 
    Copyright (c) 1984,  Alan E. Klietz.   All Rights reserved.

	These are of set of routines to build and send messages to
the random file subtask (random_msgs) to manipulate random files.
*/

#include "/std/def.h"
#include "random_file.h"
#include "random_msgs.h"

static	unsigned_word	rfm_tid;	/* Tid if child rfm task */
char	*bad_send = "fatal:rfm - unable to send to random_msgs";



FILE	*random_open(filename)
char	*filename;
{
	struct	ra_openmsg	openmsg;
	struct	ra_callcomplete callcomplete;

	check_tid();	/* spawn random_msgs if necessary */

	openmsg.ra_checkword = N_CHECK;
	openmsg.ra_msgnum = RA_OPENMSG;
	strcpy(openmsg.ra_filename, filename);
	
	if (send(rfm_tid, &openmsg, &callcomplete, sizeof(openmsg)) == 0)   
		crash(bad_send);

	return(callcomplete.ra_complete_code);
}



FILE	*random_create(filename, initsize, cachesize)
char	*filename;
unsigned_word	initsize,	cachesize;
{
	struct	ra_createmsg	createmsg;
	struct	ra_callcomplete	callcomplete;

	check_tid();	/* spawn random_msgs if necessary */

	createmsg.ra_checkword = N_CHECK;
	createmsg.ra_msgnum = RA_CREATEMSG;
	strcpy(createmsg.ra_filename, filename);
	createmsg.ra_initsize = initsize;
	createmsg.ra_cachesize = cachesize;
	
	if (send(rfm_tid, &createmsg, &callcomplete, sizeof(createmsg)) == 0)
		crash(bad_send);

	return(callcomplete.ra_complete_code);
}



random_close(fp)
FILE	*fp;
{
	struct	ra_closemsg		closemsg;
	struct	ra_callcomplete	callcomplete;

	closemsg.ra_checkword = N_CHECK;
	closemsg.ra_msgnum = RA_CLOSEMSG;
	closemsg.ra_fp = fp;

	if (send(rfm_tid, &closemsg, &callcomplete, sizeof(closemsg)) == 0)
		crash(bad_send);

	return;
}



random_compact(fp)
FILE 	*fp;
{
	struct	ra_compactmsg	compactmsg;
	struct	ra_callcomplete	callcomplete;

	compactmsg.ra_checkword = N_CHECK;
	compactmsg.ra_msgnum = RA_COMPACTMSG;
	compactmsg.ra_fp = fp;

	if (send(rfm_tid, &compactmsg, &callcomplete, sizeof(compactmsg)) == 0)
		crash(bad_send);

	return;
}



unsigned_word	random_read(fp, index, data, maxsize)
FILE	*fp;
word	index;
char	*data;
unsigned_word	maxsize;
{
	extern	unsigned_word	random_generic_read();

	return(random_generic_read(fp, index, &data, maxsize, FALSE));
}



unsigned_word	random_allocread(fp, index, data, maxsize)
FILE	*fp;
word	index;
char	**data;	/* pointer to data pointer */
unsigned_word	maxsize;
{
	extern	unsigned_word	random_generic_read();

	return(random_generic_read(fp, index, data, maxsize, TRUE));
}



unsigned_word	random_generic_read(fp, index, data, maxsize, ALLOCFLAG)
FILE	*fp;
word	index;
char	**data;
unsigned_word	maxsize;
boolean	ALLOCFLAG;
{
	struct	ra_readmsg		readmsg;
	struct	ra_generic_msg	generic_reply;	
	struct	ra_generic_msg	*reply_buffer;
	struct	ra_proceed		proceed;
	unsigned_word	finalsize;

	readmsg.ra_checkword = N_CHECK;
	readmsg.ra_msgnum = RA_READMSG;
	readmsg.ra_index = index;
	readmsg.ra_fp = fp;
	readmsg.ra_maxsize = maxsize;

	if (send(rfm_tid, &readmsg, &generic_reply, sizeof(readmsg)) == 0)
		crash(bad_send);

	if (generic_reply.ra_msgnum == RA_READSIZEMSG) {
		reply_buffer = alloc(generic_reply.ra_readsize + 
						sizeof(struct ra_readmsg_reply));
		proceed.ra_checkword = N_CHECK;
		proceed.ra_msgnum = RA_PROCEEDMSG;
		if (send(rfm_tid, &proceed, reply_buffer, sizeof(proceed)) == 0)
			crash(bad_send);
		
		finalsize = reply_buffer->ra_readsize;
		if (ALLOCFLAG) 
			*data = alloc(finalsize);
		copy(*data, reply_buffer->ra_read_data, finalsize);
		free(reply_buffer);
		return(finalsize);
	}
	else {
		if (ALLOCFLAG) 
			if (generic_reply.ra_readsize > 0)
				*data = alloc(generic_reply.ra_readsize);
			else
				*data = NULL;
		if (generic_reply.ra_readsize > 0)
			copy(*data, generic_reply.ra_read_data, generic_reply.ra_readsize);
		return(generic_reply.ra_readsize);
	}
}



unsigned_word	random_write(fp, index, data, size)
FILE	*fp;
word	index;
char	*data;
unsigned_word	size;
{
	struct	ra_writemsg			*writemsg;
	struct	ra_writesizemsg		writesizemsg;
	struct	ra_writemsg_reply	writemsg_reply;
	struct	ra_proceed			proceed;

	if (size > MAX_XFER_SIZE) { /* if too big for without warning */
		writesizemsg.ra_checkword = N_CHECK;
		writesizemsg.ra_msgnum = RA_WRITESIZEMSG;
		writesizemsg.ra_writesize = size;

		if (send(rfm_tid, &writesizemsg, &proceed, sizeof(writesizemsg))
				== 0)
			crash(bad_send);
	}

	writemsg = alloc(size + sizeof(struct ra_writemsg));
	writemsg->ra_checkword = N_CHECK;
	writemsg->ra_msgnum = RA_WRITEMSG;
	writemsg->ra_index = index;
	writemsg->ra_fp = fp;
	writemsg->ra_data_writesize = size;
   	copy(writemsg->ra_write_data, data, size);
	if (send(rfm_tid, writemsg, &writemsg_reply, size +
			sizeof(struct ra_writemsg)) == 0)
		crash(bad_send);

	free(writemsg);
	return(writemsg_reply.ra_writesize);
}	


check_tid()
{
	if (rfm_tid == 0) /* if not initialized */
		if ((rfm_tid = spawn("/std/random_msgs", 0, 0)) == 0)
			crash("fatal:rfm - unable to call random_msgs.\n");
}
