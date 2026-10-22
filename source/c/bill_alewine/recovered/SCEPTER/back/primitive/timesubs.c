/*		timesubs.o - various time related subs.
		84/08/01.	A. E. Klietz.
*/

#define NONSTANARD

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"


boolean notimeout(plyr)
/* This routine is obsolete and should go away */
{
	return(TRUE);
}


boolean readycheck(lastatk)
TI_TIME	*lastatk;
/* return TRUE if user is not ready for action. */
{
	register	i;

	if ((i = secdiff(&realtime, lastatk)) > 1)
		mwrite(My_mask, "Not ready!  Wait %d more seconds.\n", i);
	else
		if (secdiff(&realtime, &user->us_drunk) >= 0)
			mwrite(My_mask, desc(1296));
			/* You're too smashed to do anything!\n */
		else
			return(TRUE);

	return(FALSE);
}


addtime(time, secs, newtime)
TI_TIME 	*time;
unsigned	secs;
TI_TIME 	*newtime;
/* Compute   newtime = time + secs */
{

	if (secs > 256)
		mwrite(My_mask, "Sce000 - Excessive add time of %d.\n", secs);

	newtime->ti_lower = time->ti_lower;
	newtime->ti_upper = time->ti_upper + secs;
}
