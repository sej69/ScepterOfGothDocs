/*		command - combat commands.
		84/05/06.	A. E. Klietz
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"


struct cm_cmdlist {
	char	*cm_command;
	word	cm_index;
}	cmdlist[] = {
	"accelerate",	1,
	"brief", 6,
	"eject", 7,
	"end", 7,
	"f", 8,
	"fire", 11,
	"g",	5,
	"l",	11,
	"launch", 12,
	"list", 13,
	"m",	12,
	"p",	3,
	"quit", 7,
	"rotate", 14,
	"s",	4,
	"send", 15,
	"set",	2,
	"show", 16,
	"v",	10,
	"z", 9,
	
};

#define  CMDLIST_LEN	(sizeof(cmdlist) / sizeof(struct cm_cmdlist))

do_command(buffer)
/* Process all commands */
char	*buffer;
{
	struct	sh_ship		*shptr;
	char	wrd[BUFLEN], wrd2[BUFLEN];	
	word	cmdnum, oldparseloc;
	extern	word	cparseloc;
	double	f,g;
	word	i,j;

shptr = &ship[My_ship];

creset_parse();
while (oldparseloc = cparseloc, cparse(buffer, wrd) >= 0) {

	cmdnum = binsearch_ptr(wrd, &cmdlist[0].cm_command,		
		sizeof(struct cm_cmdlist), CMDLIST_LEN);
	if (cmdnum >= 0) /* convert to index */
		cmdnum = cmdlist[cmdnum].cm_index;

	switch (cmdnum) {

case -2: mwrite(My_mask,"\"%s\" is not unique.\n", wrd);
		 break;

case -1: mwrite(My_mask, "\"%s\" - no such command.\n", wrd);
		 break;

case 2: /* set */
		set(buffer);
		break;

case 16: /* show */
		show(buffer);
		break;

case 1: /* accelerate */
	if (dparse(buffer,wrd) <= 0 || (f = atof(wrd)) == 0.0) {
		shptr->sh_thrust = 0.0;
		shptr->sh_thrust_timeleft = 0;
		if (!user->us_brief)
			mwrite(My_mask,"Acceleration stopped.\n");
		break;
	}
	if ((f = atof(wrd)) < -5)
		f = -5;
	else
		if (f > 5)
			f = 5;

	if (shptr->sh_engine_blown) {
		if (!user->us_brief && (f < -0.5 || f > 0.5))
			mwrite(My_mask,"Impulse power only.  Accelerating at 0.5km/sec/sec\n");

		if (f < -0.5)
			f = -0.5;
		else       
			if (f > 0.5)
				f = 0.5;
	}

	if (dparse(buffer,wrd) <= 0 || (i = atoi(wrd)) == 0)
		i = 1;
	else
		if (i < 0) {
			shptr->sh_thrust = 0.0;
			shptr->sh_thrust_timeleft = 0;
			if (!user->us_brief)
				mwrite(My_mask,"Bad acceleration time.\n");
			break;
		}
		else
			if (i > 300) {
				if (!user->us_brief)
					mwrite(My_mask,"WARNING - engines may overheat.\n");
				i = 300;
			}

	if (!user->us_brief)
		mwrite(My_mask, "Firing engines.\n");
    shptr->sh_thrust = f;
	shptr->sh_thrust_timeleft = i;
	break;
	
		
case 6: /* brief */
	user->us_brief = !user->us_brief;
	break;

case 4: /* shield */
case 8: /* full */
case 10: /* vector */
case 3: /* repair */
case 9: /* zero */
case 5: /* display */
	cparseloc = oldparseloc;
	set(buffer);
	break;


case 14: /* rotate */
	if (dparse(buffer,wrd) <= 0 || (f = atof(wrd)) == 0.0) {
	shptr->sh_rotate = 0.0;
		shptr->sh_rotate_timeleft = 0;
		if (!user->us_brief)
			mwrite(My_mask, "Rotation stopped.\n");
		break;
	}

	if ((f = atof(wrd)) <= -360.0)
		f = -359.9;
	else
		if (f >= 360.0)
			f = 359.9;
	
	if (dparse(buffer,wrd) <= 0 || (i = atoi(wrd)) == 0)
		i = 6;
	else
		if (i < 1 || i > 6) {
			if (!user->us_brief)
				mwrite(My_mask, "Bad rotation rate. 0 < r <= 6 degrees/sec.\n"); 
			shptr->sh_rotate = 0.0;
			shptr->sh_rotate_timeleft = 0;
			break;
		}

	if (!user->us_brief)
		mwritef(My_mask, "Rotation will complete in %5.1f secs.\n",
			fabs(f) / i);

	shptr->sh_rotate_timeleft = (int) ceil(fabs(f) / i);
    shptr->sh_rotate = (f * dpie() / 180.0) / shptr->sh_rotate_timeleft;
	break;


case 7: /* end */

		if (strlen(wrd) < 3) {
			mwrite(My_mask,"Please spell that out completely.\n");
			break;
		}

{       /* check for enemy ships within 30000km */
		register	i;
		boolean		too_close = FALSE;
		
		for (i=0; i < MAX_SHIPS; ++i)
			if (ship[i].sh_exist && i != My_ship) {
				float	i_dist, j_dist;

				i_dist = shptr->sh_disp_i - ship[i].sh_disp_i;
				j_dist = shptr->sh_disp_j - ship[i].sh_disp_j;
				if (mag(i_dist, j_dist) < 30000.0) { /* if too close */
					too_close = TRUE;
					break;
				}
			}
		mwrite(My_mask, "\007DESTRUCT ORDER VERIFIED...\n");
		if (too_close) {
			mwrite(My_mask,"Destruct abort - another ship is within 30,000km.\n");	
			break;
		}
		mwrite(My_mask, "\nYou eject safely from your ship.  As you drift away,\n");
		mwrite(My_mask,"you watch your craft blossom in a silent flower of light.\n\n");
		mwrite(Allbutme_mask,"### %s (%d) just ejected from %s ship.\n",
			genuser[My_port].gn_name, My_ship,
			gender(genuser[My_port].gn_ttynum) ? "her" : "his");
		daymsg(My_ship, "EJC", -1);
		shptr->sh_exist = FALSE;
		genuser[My_port].gn_logout = TRUE;
		return;
}



case 13: /* list */
	mwrite(My_mask,"\nThe available commands are:\n");
	{
		for (i=0; i < CMDLIST_LEN; ++i)
			if (strlen(cmdlist[i].cm_command) > 1)
				mwrite(My_mask,"%s\n", cmdlist[i].cm_command);

	}
	mwrite(My_mask,"\n\n");
	break;

case 11: /* fire */
	laser(buffer);
	break;

case 12: /* launch */
	missle(buffer);
	break;

case 15: /* send */
{
	register	k, mask;

	mask = 0;
	if (cparseloc <= 0 || buffer[cparseloc - 1] == ' ')
		mask = Allbutme_mask;
	else
		while (cparse(buffer, wrd) > 0) {
			k = 0;
			if (wrd[0] == '0' || (k = atoi(wrd)) != 0) {
				if (k < 0 || k >= MAX_PORTS || !ship[k].sh_exist)
					mwrite(My_mask,"%d - no such ship.\n");
				else
					mask |= 1 << k;
			if (cparseloc <= 0 || buffer[cparseloc - 1] == ' ')
				break;
			else
				continue;
			}
        	
			for (k=0; k < MAX_PORTS; ++k) {
				strcpy(wrd2, genuser[k].gn_name);
				cstring_tolower(wrd2);
				if (subset(wrd, wrd2))
					break;
			}   	
			if (k < MAX_PORTS)
				mask |= 1 << k;
			else 
				mwrite(My_mask,"\"%s\" - no such user.\n", wrd);
    	
			if (cparseloc <= 0 || buffer[cparseloc - 1] == ' ')
				break;
		}

	if (cparseloc <= 0)
		mwrite(My_mask,"No text to send.\n");
	else
		mwrite(mask,"%s (%d) %s, \"%s\"\n", genuser[My_port].gn_name, My_ship, 
			(mask == Allbutme_mask ? "broadcasts" : "sends"),
			(char *) (buffer + cparseloc));
	cparseloc = -1;
	break;
}


default:
	mwrite(My_mask, "%s - command not implemented yet.\n", wrd);
	break;

		}
	}
	show_display();
}
