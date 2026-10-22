/*		combat - major combat routines.
		84/05/06.	A. E. Klietz
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

struct		us_user		*user,	user_array[MAX_PORTS];

struct		sh_ship		ship[MAX_PORTS];

word		My_ship;

do_initialize()
{
	extern	TI_TIME	lastupdate;

	time(&lastupdate);	/* initialize last update time. */
	day_init();
}


do_hangup()
{
	struct	sh_ship	*shptr;	

	My_ship = My_port;
	shptr = &ship[My_ship];

	if (!shptr->sh_exist)
		return;
	else {
		daymsg(My_ship, "HUP", -1);
		mwrite(Allbutme_mask, "### %s (%d) just hung up the phone.\n",
			genuser[My_port].gn_name,  My_ship);
		if (shptr->sh_lasthit != -1) { /* if hit before */
			struct	sh_ship	*othersh;

			othersh = &ship[shptr->sh_lasthit];
			if (othersh->sh_exist) { /* if other guy still exists */
				mwrite((1 << shptr->sh_lasthit),
					"You got credit for the kill!\n");
				kill_credit(shptr, othersh);
			}
		}
	}
	zero(shptr, sizeof(struct sh_ship));
	user->us_display_range = 60000L;
	user->us_vector = -1;
	shptr->sh_lasthit = -1;
}
				
				

do_input(text)
char	*text;
{
	struct		sh_ship		*shptr;
	float		heading, range;
	char		buffer[BUFLEN], buf2[BUFLEN];
	word		i;
	
	My_ship = My_port;
	shptr = &ship[My_ship];
	if (user->us_entry == US_ENTERNAME) { /* if new user */
		if (text[0] != '\0') { /* if name specified */
			creset_parse();
			cupper_parse(text, text);
			text[11] = '\0';
		}
		else
			strcpy(text, get_tty_login_name(genuser[My_port].gn_ttynum));

		strcpy(buffer, text);
		cstring_tolower(buffer);
		for (i=0; i < MAX_PORTS; ++i) {
			strcpy(buf2, genuser[i].gn_name);
			cstring_tolower(buf2);
			if (equal(buf2, buffer))
				break;
		}
		if (i < MAX_PORTS) { /* if duplicate name */
			mwrite(My_port,"Duplicate name, choose another");
			return;
		}
			
		text[0] = toupper(text[0]);
		strcpy(genuser[My_port].gn_name, text);
		mwrite(All_mask,"### %s (%03u-%04d) just appeared on ship #%d.\n",
			genuser[My_port].gn_name, (unsigned) genuser[My_port].gn_useracct,
			genuser[My_port].gn_userindex, My_ship);
		zero(shptr, sizeof(struct sh_ship));
		range = rnd() * 30000.0 + 10000.0;
		heading = rnd() * 2 * dpie();
		shptr->sh_disp_i = range * cos(heading);
		shptr->sh_disp_j = range * sin(heading);
		heading = rnd() * 2 * dpie();
		shptr->sh_bow_i = cos(heading);
		shptr->sh_bow_j = sin(heading);
		shptr->sh_exist = TRUE;
		shptr->sh_power = 35000L;

{       /* check for enemy ships within 30000km and turn on shields */
		register	i;
		
		for (i=0; i < MAX_SHIPS; ++i)
			if (ship[i].sh_exist && i != My_ship) {
				float	i_dist, j_dist;

				i_dist = shptr->sh_disp_i - ship[i].sh_disp_i;
				j_dist = shptr->sh_disp_j - ship[i].sh_disp_j;
				if (mag(i_dist, j_dist) < 30000.0) { /* if too close */
					shptr->sh_shield[0].sh_pwr = 25; /* shields up! */
					shptr->sh_shield[1].sh_pwr = 25; 
					break;
				}
			}
}

		shptr->sh_ship_repair = 33;
		shptr->sh_shield[0].sh_repair = 33;
		shptr->sh_shield[1].sh_repair = 33;
		shptr->sh_missles = 25;
		shptr->sh_lasthit = -1;
		user->us_display_range = 60000L;
		user->us_vector = -1;

		show_display();
		mwrite(My_mask,"Action");
		user->us_entry = US_COMMAND;
		return;

	} /* ship initialization complete */

	/* otherwise do command */
	do_command(text);
	if (!genuser[My_port].gn_logout)
		if (user->us_entry == US_COMMAND)
			mwrite(My_mask, user->us_brief ? "Do" : "Action");		
}     
