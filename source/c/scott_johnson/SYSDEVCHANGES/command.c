y_mask, "%2d/%2d/%-2d %2d ", shptr->sh_missle1_wait,
		shptr->sh_missles, shptr->sh_missle2_wait,
		shptr->sh_laser_wait);

	mwritef(My_mask,"%4.1f/", shptr->sh_thrust);
	mwrite(My_mask,"%-3d %4d/%-3d %4d\n", 
		shptr->sh_thrust_timeleft,
		(int) (shptr->s