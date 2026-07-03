/*
 * common.h
 *
 * $Log: common.h,v $
 * Revision 1.1  1996/04/22  23:09:13  shawn
 * Initial revision
 *
 */

#ifdef ERUG
#define	BINDIR	"/home/wm/erug"
#define	TMPDIR	"/home/wm/erug"
#define	WORKDIR	"/home/wm/erug"
#define	SERVER	"erugd"
#else
#define	BINDIR	"/home/wm/bin"
#define	TMPDIR	"/tmp"
#define	WORKDIR	"/home/wm/etc/scepter"
#define	SERVER	"scepterd"
#endif
