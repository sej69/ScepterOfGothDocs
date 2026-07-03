
/*
 *	Standard header file
 *	85.10.11.  Alan Klietz
 */
#ifndef _STD

#include <sys/types.h>

#define	TRUE		1
#define	FALSE		0
#define BUFLEN		80

#define VOID		(void)
#define copy(X,Y,Z)	memcpy(X,Y,Z)
#define real_system(X)	system(X)
#define streql(X)	equal(X)
#define zero(X,Y)	memset(X,0,Y)

#define byte		char
#define	boolean		char
#define bitfield  	unsigned long

#define RESTRICTED_GID	-1
#define GUEST_GID	-2
#define STAFF_GID	-3

#ifndef NULL
#define NULL		((char *) 0)
#endif

/* Local library */
extern boolean equal();
extern char *xalloc();
extern char *program_name();
extern boolean isseparator();
extern boolean equal();
extern int (*error_handler())();
extern boolean substring();

#define index iindexx  /* Avoid confusion with 4.2bsd index() */

#define _STD	1
#endif
