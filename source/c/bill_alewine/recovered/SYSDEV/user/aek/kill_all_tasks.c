/*		kill_all_tasks - Systematically kill everything except caller.
		83/11/12.  A. E. Klietz.

		This nasty program sends HANGUP signals to all ttys except the
calling tty, and sends BREAK signals to all tasks connected to ports
17-24 except the calling port (usually /sys/accounting)

*/
#include <stdio.h>
#include "aekdef.h"
#include "globdef.h"    /* NUMDEVICES */

#define EXC_HANGUP 1
#define EXC_BREAK 2
#define IDLE   0x0004

extern unsigned My_tid;
extern char My_tty, User_num;

kill_all_tasks()
{
	int		i, count = 0;
	int		tid;

	if (User_num != -1)
		error("fatal:kill - illegal access.\n");

	print("Hanging up all ttys...  ");
	for (i=0; i < NUMDEVICES; i++)
		if (i != My_tty && (tid = tty_tid(i)) != IDLE && tid != -1) {
			print("Hangup tty%d.\n",i);
			set_exception(tid, EXC_HANGUP, 0);  /* force hangup */
			count++;
		}

	sleep(count*10);	/* wait for hangups to complete. */	

	print("Breaking all ports... ");
	for (i=17; i <= 24; i++)
		if ((tid = detach(i)) != 0 && tid != My_tid) {
			print("Breaking port %d.\n",i);
			set_exception(tid, EXC_BREAK, 0);
		}

	print("All tasks stopped.\n");
}
