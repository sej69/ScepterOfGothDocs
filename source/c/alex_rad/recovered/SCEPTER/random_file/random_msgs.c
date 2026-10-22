/*		random_msgs - Random file message handler
		84/04/28.	  A. E. Klietz.

 
       Copyright (c) 1984, Alan E. Klietz.  All Rights reserved.
 
	This is the main loop that receives messages and executes
the proper routine in response. */

#include "/std/def.h"
#include "random_file.h"
#include "random_msgs.h"


main()
{
	unsigned_word	tid;
	word			size;
	struct	ra_callcomplete	callcomplete;
    struct	ra_generic_msg 	generic_msg;
	struct	ra_generic_msg	*reply_msg;
	char			*data;

	setup("random_file");
	for (;;) {	/* loop forever */
		breakable();
		if ((tid = receive(0, &generic_msg, sizeof(generic_msg))) == 0)
			/* if received failed */
			crash("fatal:random_msgs - bad receive.\n");

		unbreakable();
		
		if (generic_msg.ra_checkword != N_CHECK) {
			errlog("warn:random_msgs - bad checkword.\n");
			continue;
		}

		switch (generic_msg.ra_msgnum) {

case RA_READMSG:
	size = random_read(generic_msg.ra_fp, generic_msg.ra_index,			
			              &data, generic_msg.ra_maxsize);
	if (size > MAX_XFER_SIZE) { /* if too big for without warning */
		reply_msg = alloc(sizeof(struct ra_readsize_reply));
		reply_msg->ra_checkword = N_CHECK;
		reply_msg->ra_msgnum = RA_READSIZEMSG;
		reply_msg->ra_readsize = size;
	
		reply(tid, reply_msg, sizeof(struct ra_readsize_reply));
		free(reply_msg);
		if ((tid = receive(tid, &generic_msg, sizeof(generic_msg))) == 0) 
			crash("fatal:random_msgs - bad readproceed receive.\n");
		
		if (generic_msg.ra_checkword != N_CHECK || generic_msg.ra_msgnum != 
				RA_PROCEEDMSG) {
			errlog("warn:random_msgs - bad readproceed.\n");
			continue;
		}
	}

	if (size == 0) /* if no data allocated */
		data = alloc(sizeof(struct ra_readmsg_reply));

	data->ra_checkword = N_CHECK;
	data->ra_msgnum = RA_READMSG;
	data->ra_readsize = size;
	reply(tid, data, sizeof(struct ra_readsize_reply) + size);
	free(data);
	break;
                 

case RA_WRITESIZEMSG: /* upcoming write */

	reply_msg = alloc(sizeof(struct ra_proceed));
	reply_msg->ra_checkword = N_CHECK;
	reply_msg->ra_msgnum = RA_PROCEEDMSG;
	reply(tid, reply_msg, sizeof(struct ra_proceed));
	free(reply_msg);
	data = alloc(generic_msg.ra_writesize + sizeof(struct ra_writemsg));

	if ((tid = receive(tid, data, generic_msg.ra_writesize +
			sizeof(struct ra_writemsg))) == 0) 
		crash("fatal:random_msgs - bad write receive.\n");
	
	size = random_write(data->ra_fp, data->ra_index, data->ra_write_data,
			data->ra_data_writesize);
		
	free(data);
	reply_msg = alloc(sizeof(struct ra_writemsg_reply));
	reply_msg->ra_checkword = N_CHECK;
	reply_msg->ra_msgnum = RA_WRITEMSG;
	reply_msg->ra_writesize = size;
	reply(tid, reply_msg, sizeof(struct ra_writemsg_reply));
	free(reply_msg);
	break;


case RA_WRITEMSG: /* write */
	
	size = random_write(generic_msg.ra_fp, generic_msg.ra_index,
			generic_msg.ra_write_data, generic_msg.ra_data_writesize);
		
	reply_msg = alloc(sizeof(struct ra_writemsg_reply));
	reply_msg->ra_checkword = N_CHECK;
	reply_msg->ra_msgnum = RA_WRITEMSG;
	reply_msg->ra_writesize = size;
	reply(tid, reply_msg, sizeof(struct ra_writemsg_reply));
	free(reply_msg);
	break;

case RA_OPENMSG: /* open */
	callcomplete.ra_complete_code = random_open(generic_msg.ra_filename);
	reply(tid, &callcomplete, sizeof(callcomplete));
	break;

case RA_CREATEMSG: /* create */
	callcomplete.ra_complete_code = random_create(generic_msg.ra_filename,
												  generic_msg.ra_initsize,
										 	      generic_msg.ra_cachesize);
	reply(tid, &callcomplete, sizeof(callcomplete));
	break;

case RA_CLOSEMSG: /* close */
	random_close(generic_msg.ra_fp);
	callcomplete.ra_complete_code = TRUE;	/* always */
	reply(tid, &callcomplete, sizeof(callcomplete));
	break;

case RA_COMPACTMSG: /* compact */
	random_compact(generic_msg.ra_fp);
	callcomplete.ra_complete_code = TRUE; /* always */
	reply(tid, &callcomplete, sizeof(callcomplete));
	break;

		} /* end switch */
	} /* end for */
} /* end main */
