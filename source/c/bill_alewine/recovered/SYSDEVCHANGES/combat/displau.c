/*		display - combat displays.
		84/05/06.	A. E. Klietz
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

show_display()
{
	register	i;
	register	struct	sh_ship		*shptr;
	float		i_vec, j_vec, m;
	boolean		flag = FALSE;

	if (!user->us_brief)
		mwrite(My_mask, "Sp Dmg S1 S2 Speed Course Azmuth  Range Heading\n");

	if (user->us_vector >= 0) /* if vectored */
		if (ship[user->us_vector].sh_exist) {
			i_vec = ship[My_ship].sh_disp_i - ship[user->us_vector].sh_disp_i;
			j_vec = ship[My_ship].sh_disp_j - ship[user->us_vector].sh_disp_j;
			show_line(user->us_vector, mag(i_vec, j_vec));
			show_line(My_ship, 0);
		}
		else
			user->us_vector = -1;  /* disable vector */

	if (user->us_vector < 0)
		for (i = 0; i < MAX_SHIPS; ++i) {
			if (ship[i].sh_exist) {
				i_vec = ship[My_ship].sh_disp_i - ship[i].sh_disp_i;
				j_vec = ship[My_ship].sh_disp_j - ship[i].sh_disp_j;
				if ((long) (m = mag(i_vec, j_vec)) <= user->us_display_range ||
						fabs(azmuth(i, My_ship)) < 0.0872664 ||
						fabs(azmuth(My_ship, i)) < 0.0872664)
					show_line(i, m);
				else 
					mwrite(My_mask,"%2d",i);
			}   	
		}
	else {       	
		for (i = 0; i < MAX_SHIPS; ++i)
			if (ship[i].sh_exist && i != My_ship && i != user->us_vector) 
				if (fabs(azmuth(i, My_ship)) < 0.0872664) {
					mwrite(My_mask,"(%d/", i);
					mwritef(My_mask,"%-6.1f ", azmuth(i, My_ship)
						* 180 / dpie());
					flag = TRUE;
				}
		if (flag)
			mwrite(My_mask,"\n");
	}
		
	if (!user->us_brief)
		mwrite(My_mask, "Energy Dmg1 Dmg2 T1/M1/T2 T3 Acel/Tim  Deg/Tim Heat\n");

	shptr = &ship[My_ship];
	mwritef(My_mask, "%6ld %3d%% %3d%% ", shptr->sh_power,
		shptr->sh_shield[0].sh_shield_dmg,
		shptr->sh_shield[1].sh_shield_dmg);

	mwrite(My_mask, "%2d/%2d/%-2d %2d ", shptr->sh_missle1_wait,
		shptr->sh_missles, shptr->sh_missle2_wait,
		shptr->sh_laser_wait);

	mwritef(My_mask,"%4.1f/", shptr->sh_thrust);
	mwrite(My_mask,"%-3d %4d/%-3d %4d\n", 
		shptr->sh_thrust_timeleft,
		(int) (shptr->sh_rotate * shptr->sh_rotate_timeleft * 180 / dpie()),
		shptr->sh_rotate_timeleft,
		shptr->sh_heat);
}


show_line(shipnum, magnitude)
word	shipnum;
double	magnitude;
{
	register	struct	sh_ship	*shptr;
	float		i_vec, j_vec, th, m;

	shptr = &ship[shipnum];

	mwrite(My_mask, "%2d %2d%% %2d %2d ", shipnum,
		shptr->sh_ship_dmg, shptr->sh_shield[0].sh_pwr -
							shptr->sh_shield[0].sh_shield_dmg / 4,
							shptr->sh_shield[1].sh_pwr -
							shptr->sh_shield[1].sh_shield_dmg / 4);
	m = mag((i_vec = shptr->sh_vel_i), (j_vec = shptr->sh_vel_j));
	mwritef(My_mask, "%5.0f ", m);
	if (m >= .001) {
		i_vec /= m;
		j_vec /= m;
		if ((m = i_vec * shptr->sh_bow_i + j_vec * shptr->sh_bow_j) >= 1.0)
			th = 0.0;
		else {
			th = acos(m);
    		if (j_vec * shptr->sh_bow_i < i_vec * shptr->sh_bow_j)
				th = -th;
		}
	}
	else
		th = 0.0;

	mwritef(My_mask, "%6.1f ", th * 180 / dpie());
	if (My_ship == shipnum) { /* if same ship */
		mwrite(My_mask, "\n");
		return;
	}

	mwritef(My_mask, "%6.1f ", azmuth(My_ship, shipnum) * 180 / dpie());
	mwritef(My_mask, "%6ld ", (long) magnitude);
	mwritef(My_mask,"%6.1f\n", azmuth(shipnum, My_ship) * 180 / dpie());
}
