/*		Globdef - Global memory definitions.

		83/10/20.  A. E. Klietz.

	This procedure outlines the global memory definitions.
The global memory area is allocated by /sys/accounting.  It's
location is sent to other tasks via an accounting request.

*/

/* This definition file is dependent on aekdef.h */

#define NUMDEVICES 24

typedef struct gl_user_datum {          	
	char		gl_username[12];	 /* name of user. */
	unsigned 	gl_useracct;    	 /* user account number. (001-254) */
	unsigned    gl_userindex;	     /* user index number. (0001-9999)  */
	unsigned	gl_logintime[2];     /* time logged in (binary) */
	unsigned	gl_last_creditupdate[2];   /* date/time credits updated. */		
	unsigned	gl_binarylastcr[2];  /* time of last carriage return */
	int			gl_monitor;		     /* monitoring terminal. -1 if none. */
	int  		gl_creditbalance;    /* number of credits remaining. 
									   (negative if in debt.) */
	char		gl_ttyname[8];		 /* name of tty: $con2, $term10, etc. */
	unsigned	gl_ttyoptions;       /* tty_option bits (echo, tabs, etc.)*/
	byte		gl_apnum;			 /* current application. */
	byte		gl_dummy;			 /* alignment dummy. */
	int			gl_startcredits;	 /* starting credits at login. */
	int         gl_minute_remainder; /* internal credit_update counter */
    byte		gl_backspace;		 /* backspace character. */
	byte		gl_linedel;			 /* line-delete character. */
} GL_USER_DATA[NUMDEVICES];


typedef struct  gl_system_datum {
	unsigned	gl_binaryuptime[2]; /* up time/date in binary. */
	boolean		gl_sysopen;		/* system open flag. = TRUE if open. */
	boolean		gl_sysidle;		/* system idle flag. = TRUE if idle. */
	boolean		gl_peakflag;	/* TRUE = peak time. */
	boolean		gl_warncrash;      /* TRUE if shutdown in progress. */
} GL_SYSTEM_DATA;
