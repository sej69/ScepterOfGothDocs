/*
	Standard constant definitions
 */
#ifndef _STD

#define	NULL		0
#define	TRUE		1
#define	FALSE		0
#define	YES		1
#define	NO		0
#define MAXPATHNAME     128
#define MAXFILENAME	30
#define BUFLEN		80
#define UNAME

#define	STAFF_GID	10
#define RESTRICTED_GID	19
#define GUEST_GID	20
#define ASST_GID	21

#define byte	char
#define	boolean	char
#define bitfield  unsigned long

#ifndef _TYPES_
	#include <sys/types.h>
#endif

#ifndef unix
/*
	standard storage class definitions
 */
#define	GLOBAL	extern
#define	IMPORT	extern
#define	EXPORT	extern
#define	INTERN	static
#define	LOCAL	static
#define global	extern
#define import	extern
#define export	extern
#define intern	static
#define local	static

/*
	standard type definitions
 */
#define BOOL int
#define bool int

#ifndef lint
	#define void int
	#define VOID
#else
	#define VOID (void)

/* Local void decls for lint library */
extern void zero();
extern void copy();
extern void transform();
extern void setup();
extern void errlog();
extern void disconnect();
extern void db_close();
extern void reply();
extern void write_msg();
extern void string_tolower();
extern void reset_parse();
extern void crash();

/* void decls for UNOS/UNIX lint library */
extern void _exit();
extern void abort();
extern void alarm();
extern void chdomain();
extern void close();
extern void comerr();
extern void comerrno();
extern void cvtime();
extern void delay();
extern void ecadvance();
extern void ecclose();
extern void errmsg();
extern void errmsgno();
extern void error();
extern void execl();
extern void execle();
extern void execlp();
extern void execv();
extern void execve();
extern void execvp();
extern void exit();
extern void fclose();
extern void fendtoread();
extern void fexecl();
extern void fexecle();
extern void fexecv();
extern void fexecve();
extern void fflush();
extern void file_raise();
extern void fileseek();
extern void format();
extern void fprintf();
extern void free();
extern void ftime();
extern void geterrmsg();
extern void handle();
extern void longjmp();
extern void luwritethrough();
extern void message();
extern void movebytes();
extern void pause();
extern void pclose();
extern void perror();
extern void printf();
extern void proctime();
extern void psinfo();
extern void qsort();
extern void quicksort();
extern void raise();
extern void randomize();
extern void segunshare();
extern void setpgrp();
extern void sleep();
extern void sprintf();
extern void srand();
extern void strcat();
extern void strcatl();
extern void strcpy();
extern void strncat();
extern void strncpy();
extern void swab();
extern void sync();
#endif lint

/* Local gb library */
extern char *xalloc();
extern char *program_name();
extern boolean isseparator();
extern boolean equal();
extern int (*error_handler())();
extern boolean substring();

/* UNOS/UNIX library */
extern long absaddr();
extern char *asctime();
extern char *astod();
extern char *astof();
extern char *astoi();
extern char *astol();
extern long atol();
extern short atos();
extern char *calloc();
extern char *crypt();
extern char *ctime();
extern char *ecvt();
extern char *fcvt();
extern off_t filepos();
extern off_t filesize();
extern char *find();
extern char *gcvt();
extern short getcmask();
extern short getegid();
extern char *getenv();
extern short geteuid();
extern short getgid();
extern char *getlogin();
extern char *getpass();
extern char *gets();
extern long gettime();
extern short getuid();
extern char *getuname();
extern char *gwd();
extern boolean is_addressable();
extern boolean isatty();
extern unsigned long lrand();
extern char *malloc();
extern char *map_io_page();
extern char *mktemp();
extern void (*onintr())();
extern void (*onsuspend())();
extern boolean parseranges();
extern char *patmatch();
extern char *realloc();
extern char *segshare();
extern void (*signal())();
extern boolean streql();
extern char *strindex();
extern char *strpbrk();
extern char *strtok();
extern long strtol();
extern off_t tell();
extern time_t time();
extern long times();
extern char *timestr();
extern char *timetostr();
extern char *ttyname();

#define index iindexx  /* Avoid confusion with 4.2bsd index() */

#ifdef M68000
#define	JOS	1
#endif

#endif unix
#define _STD	1
#endif
