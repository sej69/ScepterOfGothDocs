/*		aekdef - AEK's program definitions.
		83/09/25.	A. E. Klietz.

		This file contains a list of standard definitions and 
declarations used in all of AEK's programs.

*/

#define TRUE 1
#define FALSE 0
#define boolean char
#define byte	char
#define	BUFLEN	80
#define GUEST_ACCT  1

/* undefine the silly definition of DEBUG from task.h */
#undef DEBUG

typedef		char	BUFTYPE[BUFLEN];

struct cmdlist_type
{
	char *cmd;
	int	 cmdnum;
};

static	char	 *COPYRIGHT_MSG = "Copyright (c) 1984, Alan E. Klietz.";
