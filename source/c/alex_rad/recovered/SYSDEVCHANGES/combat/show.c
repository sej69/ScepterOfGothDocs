/*			show - combat show command
			84/05/13.	A. E. Klietz.

*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

static struct cm_showlist {
	char	*cm_command;
	word	cm_index;
}	showlist[] = {
	"center", 1,
	"logfile", 4,
	"parallel", 3,
	"who", 2,
};

#define  SHOWLIST_LEN	(sizeof(showlist) / sizeof(struct cm_showlist))

show(buffer)
char	*buffer;
{
	struct	sh_ship		*shptr;
	char	wrd[BUFLEN], wrd2[BUFLEN];	
	word	shownum;
	double	f,g,th;
	longword	l;
	word	i,j;

	shptr = &ship[My_ship];

	cparse(buffer, wrd);

	shownum = binsearch_ptr(wrd, &showlist[0].cm_command,		
		sizeof(struct cm_showlist), SHOWLIST_LEN);
	if (shownum >= 0) /* convert to index */
		shownum = showlist[shownum].cm_index;

	switch (shownum) {

case -2: mwrite(My_mask,"\"%s\" is not unique.\n", wrd);
		 break;

case -1: mwrite(My_mask, "\"%s\" - no such SHOW parameter.\n", wrd);
		mwrite(My_mask, "\nValid SHOW parameters are:\n");
		for (i=0; i < SHOWLIST_LEN; ++i)
			if (strlen(showlist[i].cm_command) > 1)
				mwrite(My_mask,"%s\n", showlist[i].cm_command);
		mwrite(My_mask,"\n");
	 	break;

case 1: /* center */

	f = shptr->sh_disp_i;
	g = shptr->sh_disp_j;
	l = mag(f, g);
	if (l <= 0) /* if sitting on center */
		break;
	f /= l;
	g /= l;
	if (fabs(th = f * shptr->sh_bow_i + g * shptr->sh_bow_j) >= 1)
		th = 0;
	else {
		th = acos(th);
	    if (g * shptr->sh_bow_i < f * shptr->sh_bow_j)
			th = -th;
	}

	mwritef(My_mask,"Range %ld km,", l);
	mwritef(My_mask," Azmuth %6.1f to center.\n", th * 180 / dpie());
	if (genuser[My_port].gn_useracct == SUPER_ACCT) { /* if staff */
		mwritef(My_mask,"i-vector = %ld,  ", (long) shptr->sh_disp_i);
		mwritef(My_mask,"j-vector = %ld.\n", (long) shptr->sh_disp_j);
	}
	break;

case 2: /* who */
	mwrite(My_mask,"\n");
	if (!user->us_brief)
		mwrite(My_mask,"Shp Commander   Act-Index Kills\n");
		
	for (i=0; i < MAX_SHIPS; ++i) 
		if (ship[i].sh_exist) {
			mwrite(My_mask,"%2d  %-11s %03u-%04d   ", 
	    		i, genuser[i].gn_name,
				(unsigned) genuser[i].gn_useracct,
				genuser[i].gn_userindex);
			mwrite(My_mask,"%d\n",ship[i].sh_kills);
		}
	mwrite(My_mask,"\n");
	break;	

case 3: /* parallel */
	if (dparse(buffer, wrd) <= 0 || (i = atoi(wrd)) < 0 || i >= MAX_SHIPS
		|| !ship[i].sh_exist || i == My_ship) {
		if (!user->us_brief)
			mwrite(My_mask, "Missing or bad ship number.\n");
		break;
	}

	if (!user->us_brief) {
		mwrite(My_mask,"\nTo move parallel to ship #%d, you must move as follows:\n", i);
		mwrite(My_mask,"Sp    Rot   Spd or Rot   Spd\n");
	}

	f = ship[i].sh_vel_i - shptr->sh_vel_i;
	g = ship[i].sh_vel_j - shptr->sh_vel_j;
	
	l = mag(f, g);

	if (l < .00001) {
		mwrite(My_mask,"(Already parallel)\n\n");
		break;
	}

	f /= l;
	g /= l;
	if (fabs(th = f * shptr->sh_bow_i + g * shptr->sh_bow_j) >= 1)
		th = 0;
	else {
		th = acos(th);
	    if (g * shptr->sh_bow_i < f * shptr->sh_bow_j)
			th = -th;
	}
	mwrite(My_mask,"%2d ",i);
	mwritef(My_mask,"%6.1f ",th * 180 / dpie());
	mwritef(My_mask, "%5ld ",l);
	th = (th <= 0) ? th + dpie() : th - dpie();
	mwritef(My_mask,"%6.1f ",th * 180 / dpie());
	mwritef(My_mask,"%5ld\n\n", -l);
	break;

case 4: /* logfile */
	show_dayfile();
	break;
	
default:
	mwrite(My_mask, "%s - parameter not implemented yet.\n", wrd);
	break;

		}
}
