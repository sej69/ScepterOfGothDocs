shptr;
    word		t1;

	shptr = &ship[i];

	t1 = min(shptr->sh_thrust_timeleft, t);

	shptr->sh_disp_i += shptr->sh_vel_i * t1 +
		shptr->sh_thrust * shptr->sh_bow_i * t1 * t1 / 2.0;

	shptr->sh_disp_j += shptr->sh_vel_j * t1 +
		shptr->sh_thrust * shptr->sh_bow_j * t1 * t1 / 2.0;

	shptr->sh_vel_i += shptr->sh_thrust * shptr->sh_bow_i * t1;
	shptr->sh_vel_j += shptr->sh_thrust * shptr->sh_bow_j * t1;

	t -= t1;	

	if ((shptr->sh_thrust_timeleft -= t1) <= 0) { /* if stopped accelerating */
		shptr->sh_thrust = 0.0;
		shptr->sh_thrust_timeleft = 0;
	}
}


drift(i)
word	i;
{
	register	struct	sh_ship		*shptr;
    word		t1;

	if (t <= 0) /* if interval completed */
		return;

	shptr = &ship[i];

	t1 = t;

	shptr->sh_disp_i += shptr->sh_vel_i * t1;
	shptr->sh_disp_j += shptr->sh_vel_j * t1;

	t -= t1;
}


update_misc(i)
word	i;
{
	register	struct	sh_ship		*shptr;
	word		reptime;  /* seconds of repair time */
	word		oldheat;

	shptr = &ship[i];

	/* repair shields and ship */
	if ((reptime = t - shptr->sh_repair_wait) > 0) { /* if repairing */
		if (shptr->sh_ship_dmg > 0) {
			shptr->sh_ship_dmg -= 0.015 * reptime * shptr->sh_ship_repair;

			if (shptr->sh_ship_dmg < 0)
				shptr->sh_ship_dmg = 0;
		}

        	
		if (shptr->sh_shield[0].sh_shield_dmg > 0) {
			shptr->sh_shield[0].sh_shield_dmg -=
				0.015 * reptime * shptr->sh_shield[0].sh_repair;

			if (shptr->sh_shield[0].sh_shield_dmg < 0)
				shptr->sh_shield[0].sh_shield_dmg = 0;
        }

		if (shptr->sh_shield[1].sh_shield_dmg > 0) {
			shptr->sh_shield[1].sh_shield_dmg -=
				0.015 * reptime * shptr->sh_shield[0].sh_repair;

		if (shptr->sh_shield[1].sh_shield_dmg < 0)
				shptr->sh_shield[1].sh_shield_dmg = 0;
    	}

		shptr->sh_repair_wait = 0; 
	}
	else /* wait for repair */
		shptr->sh_repair_wait -= t;


	/* deduct power usage */
	
	shptr->sh_power -= t*(max(0, shptr->sh_shield[0].sh_pwr		
		- (int) shptr->sh_shield[0].sh_shield_dmg / 4) +
		max(0, shptr->sh_shield[1].sh_pwr 
		- (int) shptr->sh_shield[1].sh_shield_dmg / 4))/2 
		+ 25 * fabs(shptr->sh_thrust * min(t, shptr->sh_thrust_timeleft))
		+ t;

	if (shptr->sh_power <= 0) { /* if no power */
		mwrite((1 << i),"\nYou are out of power.  Abandon ship!\n\n");
		mwrite((All_mask & ~(1 << i)), "### %s (%d) just lost all power.\n",
			genuser[i].gn_name, i);
		daymsg(i, "PWR", -1);
		shptr->sh_exist = FALSE;
		genuser[i].gn_logout = TRUE;
		return;
	}

	
	/* compute heat buildup and check for explosion */
	oldheat = shptr->sh_heat;
	shptr->sh_heat += 35 * fabs(shptr->sh_thrust) 
		* min(t, shptr->sh_thrust_timeleft)
		- t * 19;
	if (shptr->sh_heat < 0)
		shptr->sh_heat = 0;

	if (shptr->sh_heat >= 8000) { /* boom! */
		shptr->sh_ship_dmg = min(99, (int) shptr->sh_ship_dmg + 10);
		shptr->sh_thrust = (8000 - oldheat)/35 * (shptr->sh_thrust < 0 
			? -1 : 