/*		validef.h - validation file layout.
		83/10/21.	A. E. Klietz.

	This file defines the format of a record in the validation file.
(/val/validuz and /val/valindx)

	This file depends on aekdef.h.

	These definitions must match with those on globdef.h.
*/

#define	  va_defined

struct	va_user_record {
	char		va_username[12];	/* codename of user. */
	char		va_hashpw[12];		/* hashed password. */
	unsigned    va_useracct;		/* user account number. (6001-6254) */
	unsigned	va_userindex;   	/* user index number. (001-999) */
	unsigned 	va_ttyoptions;      /* ECHO, TABS, etc. */
	int			va_creditbalance;   /* number of credits remaining. */
								    /*  (negative if in debt.) */
	unsigned	va_lastlogin[2];    /* date of last login. */
	boolean		va_closed;	 		/* true if account is closed. */
	byte		va_birthmonth;		/* month of birthday */
	byte		va_birthdate;		/* date in month of birthday. */
	byte		va_birthyear;		/* year of birth */
	byte		va_backspace;		/* desired backspace char. */
	byte		va_linedel;	    	/* desired line-delete char. */
	boolean		va_nocharge;		/* true if no charge to this acct. */
	char		va_stringbuf[192];	/* string buffer with 
									name
									1st addr line.
									2nd addr line.
									3rd	addr line.
						 	      	phone
									hobbies/interests  */
	boolean		va_stateres;		/* true is user lives in state (tax) */
	boolean     va_anonymous;		/* true if user wants to be anonymous */
	byte		va_rating;			/* credit rating, from 1 to 10. */
	byte		va_dummy[18];		/* dummy blank fill. */
};

/* va_user_record must be exactly* 256 characters long. */	

/* The following is a definition for an index record in the
validation index file.  /config/valindx. It must be 16 bytes long.
*/

struct vi_user_hash {
	char		vi_username[12];    /* last two chars of username are lost. */
	unsigned	vi_userindex;
	unsigned    vi_link;		   /* pointer to next index entry in hash slot. */
};
