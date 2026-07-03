/*	jourdef - journal definitions.
	85/01/04.  A. E. Klietz.
*/

#define JR_CHECKBYTE	0x6e

struct jr_journal_record {
	byte		jr_checkbyte;
	byte		jr_ttynum;
	short		jr_rate;
	time_t		jr_time;
	double		jr_credits;
	double		jr_free_credits;
	boolean		jr_adjust;	/* TRUE if credit adjustment */
	byte		jr_comment_len;
	/* comment immediately follows */
};
