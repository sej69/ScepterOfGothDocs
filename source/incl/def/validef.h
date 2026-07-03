/*		validef.h - validation file layout.
		83/10/21.	A. E. Klietz.

	This file defines the format of a record in the validation file.
*/

#define VA_DEFINED

struct	va_user_record {
	char		va_username[20];/* name of user.*/
	char		va_hashpw[20];	/* hashed password.*/
	short		va_uid;
	short		va_gid;  /* Set to -1 to delete account. */
	double		va_creditbalance;
	double		va_free_creditbalance;
	boolean		va_closed; 	/* true if account is closed. */
	boolean		va_brief;	/* true if brief mode on login. */
	short		va_bmonth, va_bdate, va_byear; /* birthdate */
	time_t		va_flatrate;
	time_t		va_lastlogin;
	boolean		va_pause;
	short		va_dummy[18];
	char		va_term[20];
	short		va_adnum;
	boolean		va_nocharge;	/* true if no charge to this acct. */
	boolean         va_anonymous;	/* true if user wants to be anonymous */
	byte		va_rating;	/* credit rating, from 1 to 10. */
	byte		va_prevbday;	/* yr of previous free bday credits */
	boolean		va_gender;	/* TRUE if male, else FALSE */
	boolean		va_dummyb;
	char		va_stringbuf[192];	/* string buffer with 
						name
						1st addr line. 
						2nd addr line.
						3rd addr line.
				 	      	phone        
						hobbies/interests */
};
