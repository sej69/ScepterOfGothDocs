/*		shoot - combat shoot'em up processor.
		84/05/06.	A. E. Klietz
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

laser(buffer)
char	*buffer;
/* shoot laser */
{
	struct	sh_ship		*shptr;
	char	wrd[BUFLEN];	
	double	width;
	word	shld, i, beam, sh, dmg1, dmg2;
	longword	dmg;
	boolean	hit = FALSE;

	shptr = &ship[My_ship];

	if (dparse(buffer, wrd) <= 0 || (beam = atoi(wrd)) <= 0) {
		if (!user->us_brief)
			mwrite(My_mask, "Missing laser energy parameter.\n");
		return;
	}

	if (dparse(buffer, wrd) <= 0 || (width = atof(wrd)) < 1.0)
		width = 1.0;

	if (width > 10.0) {
		if (!user->us_brief)
			mwrite(My_mask, "Max beam width is 10 degrees.\n");
		width = 10.0;
	}

	if (shptr->sh_laser_wait != 0) {
		if (!user->us_brief)
			mwrite(My_mask, "Lasers need %d more seconds to cool.\n",
				shptr->sh_laser_wait);
		return;
	}

	if (beam > 2000)
		beam = 2000;

	beam = minf(beam, shptr->sh_power);
	shptr->sh_power -= beam;
	shptr->sh_laser_wait = (int) (5 + beam * 0.0275);

	for (i=0; i < MAX_SHIPS; ++i) 
		if (i != My_ship && ship[i].sh_exist)
			if (azmuth(My_ship, i) <= width * dpie() / 360) { /* if hit */
				sh = (azmuth(i, My_ship) <= 0.523598776) ?
					0 : 1; /* shield 0 or shield 1? */
				dmg = (beam/width) * 12 * (33 -
					min(ship[i].sh_shield[sh].sh_pwr,(100 -
					(int) ship[i].sh_shield[sh].sh_shield_dmg) / 4));
				dmg /= 1 + mag(shptr->sh_disp_i - ship[i].sh_disp_i,
						  shptr->sh_disp_j - ship[i].sh_disp_j);

				if (dmg > 100)
					dmg = 100;
											
				if (dmg <= 0)
					continue;
				dmg1 = min((int) dmg/2, 100 - (int) ship[i].sh_ship_dmg);
				if (user_array[i].us_brief)
					mwrite((1 << i), "%s S%d lsr hit shld %d for %d%%.\n", 
						dmg1 >= 20 ? "**Blam**" : ">>Pwang<<",
						My_ship, sh+1, dmg1);
				else
					mwrite((1 << i), "%s Ship #%d laser hit shield %d for %d%% damage.\n",
						dmg1 >= 20 ? "**Blam**" : ">>Pwang<<",
						My_ship, sh+1, dmg1);

				if (user->us_brief)
					mwrite(My_mask,"Lsr hit shld %d of S%d for %d%%\n",
						sh+1, i, dmg1);
				else
					mwrite(My_mask,"Laser hit shield %d of ship #%d for %d%% damage.\n",
						sh+1, i, dmg1);

				ship[i].sh_lasthit = My_ship;
				ship[i].sh_repair_wait = 40;

				dmg2 = (sh == 0) ? dmg / 2 : dmg;
				
				ship[i].sh_shield[sh].sh_shield_dmg = min(100,
					(int) ship[i].sh_shield[sh].sh_shield_dmg + dmg2);

				hit = TRUE;

				if ((ship[i].sh_ship_dmg += dmg1) >= 100)
					killed(i); /* byebye */
			}
		
	if (!hit && !user->us_brief)
		mwrite(My_mask,"You missed!\n");
}




missle(buffer)
char	*buffer;
/* launch missle */
{
	struct	sh_ship		*shptr;
	char	wrd[BUFLEN];	
	word	sh, dmg1, dmg2;
	longword dmg;
	register	i;

	shptr = &ship[My_ship];

	if (dparse(buffer, wrd) <= 0 || (i = atoi(wrd)) < 0 || i >= MAX_SHIPS
		|| !ship[i].sh_exist || i == My_ship) {
		if (!user->us_brief)
			mwrite(My_mask, "Missing target number.\n");
		return;
	}

	if (shptr->sh_missles <= 0) { /* if no missles */
		if (!user->us_brief)
			mwrite(My_mask, "You are out of missles!\n");
		return;
	}
		
	if (shptr->sh_missle1_wait != 0 && shptr->sh_missle2_wait != 0) {
		if (!user->us_brief)
			mwrite(My_mask, "Missle tubes not yet ready.\n");
		dparse(buffer,wrd);
		return;
	}


	--(shptr->sh_missles);
	if (shptr->sh_missle1_wait == 0)
		shptr->sh_missle1_wait = 60;
	else
		shptr->sh_missle2_wait = 60;

	if (azmuth(My_ship, i) <= 0.043633232 &&
		mag(shptr->sh_disp_i - ship[i].sh_disp_i, 
			shptr->sh_disp_j - ship[i].sh_disp_j) <= 20000) { /* if hit */
		sh = (azmuth(i, My_ship) <= 0.523598776) ?
			0 : 1; /* shield 0 or shield 1? */
		dmg = (long) (102 - min(ship[i].sh_shield[sh].sh_pwr * 4,
			100 - (int) ship[i].sh_shield[sh].sh_shield_dmg));

		if (dmg > 100)
			dmg = 100;
									
		dmg1 = min((int) dmg/2, 100 - (int) ship[i].sh_ship_dmg);
		if (user_array[i].us_brief)
			mwrite((1 << i), "%s S%d msl hit shld %d for %d%%.\n", 
				dmg1 >= 20 ? "**Boom**" : ">>Pwing<<",
				My_ship, sh+1, dmg1);
		else
			mwrite((1 << i), "%s Ship #%d missle hit shield %d for %d%% damage.\n",
				dmg1 >= 20 ? "**Boom**" : ">>Pwing<<",
				My_ship, sh+1, dmg1);

		if (user->us_brief)
			mwrite(My_mask,"Msl hit shld %d of S%d for %d%%\n",
				sh+1, i, dmg1);
		else
			mwrite(My_mask,"Missle hit shield %d of ship #%d for %d%% damage.\n",
				sh+1, i, dmg1);

		ship[i].sh_lasthit = My_ship;
		ship[i].sh_repair_wait = 40;

		dmg2 = dmg / 2;
		
		ship[i].sh_shield[sh].sh_shield_dmg = min(100,
			(int) ship[i].sh_shield[sh].sh_shield_dmg + dmg2);

		if ((ship[i].sh_ship_dmg += dmg1) >= 100)
			killed(i); /* byebye */
	}
	else /* missed */
		if (!user->us_brief)
			mwrite(My_mask, "Missle missed!\n");

}



killed(i)
word	i;
{
	mwrite(All_mask,"### %s's ship was just ", genuser[i].gn_name);

	switch ((int) (rnd() * 4.0)) {

case 0: mwrite(All_mask,"blown-up");
		break;

case 1: mwrite(All_mask,"blasted");
		break;

case 2: mwrite(All_mask,"destroyed");
		break;

case 3: mwrite(All_mask,"vaporized");
		break;
	}

	mwrite(All_mask," by %s. (%d)\n",
		genuser[My_port].gn_name, My_ship);

	mwrite((1 << i), "You just bought the farm.\n\n");
	daymsg(i, "DD", My_ship);
	kill_credit(&ship[i], &ship[My_ship]);
	ship[i].sh_exist = FALSE;
	genuser[i].gn_logout = TRUE;
}
