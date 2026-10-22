fine MODIFY_FLAGS	26
#define SET_TTY			27
#define TTY_ROOT_TID	28
#define RELAY			29


/*
 * Task States
 */

#define DEAD			0
#define READY			1
#define SEND_BLOCKED	2
#define RECEIVE_BLOCKED	3
#define REPLY_BLOCKED	4
#define HELD			5


/*
 * Task flag bits
 */

#define TO_BE_HELD		0x0001
#define DOOMED			0x0002
#define ESCAPED_SHELL	0x0004
#define FETAL_SHELL		0x0008
#define CONCURRENT		0x0010
#define ADMIN			0x0020
#define FIXED			0x0040	/* Shared by code flags */
#define PRIVILEGED		0x0080	/* Shared by code flags */
#define INFORM			0x0100
#define WAIT			0x2000
#define PART_NDP_SAVE	0x4000
#define FULL_NDP_SAVE	0x8000


/*
 * Code record flag bits
 */

#define NEEDS_NDP		0x01
#define SHAREABLE		0x02
#define SET_OWNER		0x04


/*
 * System flags in sflags f