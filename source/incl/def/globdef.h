/*		Globdef - Global accounting definitions.
		83/10/20.  A. E. Klietz.
*/

#define MAXTTYS	36

#define GL_DEFINED

struct gl_user_datum {          	
	char	 	gl_username[12];	 /* name of user. */
	unsigned short	gl_uid;	
	unsigned short  gl_gid;
	int		gl_pid;
	time_t		gl_logintime;
	time_t		gl_last_creditupdate;   /* time credits updated. */
	time_t		gl_flatrate;
	double  	gl_creditbalance;    /* number of credits remaining. */
	double		gl_startcredits; /* starting credits at login. */
	double		gl_free_creditbalance;
	double		gl_free_startcredits;
	boolean		gl_b1200;	 /* True if 1200 baud */
	boolean		gl_b2400;	  /* True if 2400 baud */
	byte		gl_apnum;
	boolean		gl_brief;
	boolean		gl_pause;	/* True if pause mode in mail */
	char		gl_apname[20];
	int		gl_adnum;
	char		gl_term[20];
};
