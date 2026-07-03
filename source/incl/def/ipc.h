/*  ipc.h - interprocess communication definitions
    84/12/13.  A. E. Klietz.
*/

/* The checkwords should be something non-trivial, but any number will do. */
#define IP_STRUCT_CHECKWORD 0xA1D3
#define IP_SEND_CHECKWORD   0xCF5E
#define IP_REPLY_CHECKWORD  0x918F

#define MULTI   "/usr/multi/multi"  /* Multi-user initialization program */
#define EVENTFILE "/usr/ipc/%s.ec"  /* Read/write event count lock */
#define DEATHFILE "/usr/ipc/%s.de"  /* Advanced by MULTI when receiver dies */
#define WMBX      "/usr/ipc/%s.wr"  /* Write mbx.  send -> receive */
#define RMBX      "/usr/ipc/%s.rd"  /* Read mbx.   reply -> send */

struct ip_ipc {  /* ipc communication table */
	int	ip_checkword;
	int	ip_event;
	int	ip_wmbx;
	int	ip_rmbx;
	int	ip_deathevent;
#ifdef KLUDGE
	char	ip_wmbx_name[128];
	char	ip_rmbx_name[128];
#endif
};

struct ph_packet_header { /* packet header */
	int	ph_checkword;
	int	ph_pid;
	int	ph_length;
};
