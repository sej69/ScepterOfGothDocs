/*			update - periodic combat update
			84/05/08.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"


TI_TIME		lastupdate;  /* time of last update, in seconds. */

static		word	t;

update()
/* Set all vectors, rotations, accelerations, and heat up to date. */
{
	word	i;
	register	struct	sh_ship		*shptr;
	TI_TIME		bindate;
	word		interval;
	boolean		rotflag;

	time(&bindate);
	if ((interval = secdiff(&lastupdate, &bindate)) <= 0) /* if bad time */
		return;

	time(&lastupdate); 
	for (i=0; i < MAX_SHIPS; ++i)
		if (ship[i].sh_exist) {
			shptr = &ship[i];
			t = interval;
{    /* reset azmuths to out-of-date */
			register	j;
		
			for (j=0; j < MAX_SHIPS; ++j)
				shptr->sh_good_azmuth[j] = FALSE;
}                    
			update_misc(i);  /* update miscellaneous things */
			rotflag = FALSE;
			if (shptr->sh_thrust_timeleft > 0 &&
					shptr->sh_rotate_timeleft > 0) {
				rot_accel(i);  /* rotate and accelerate */
				rotflag = TRUE;
			}
			
			if (t > 0 && shptr->sh_rotate_timeleft > 0) {
				rot(i);  /* rotate */
				rotflag = TRUE;
			}

			/* any correction of bow vectors should go here! */

			if (t > 0 && shptr->sh_thrust_timeleft > 0)
				accel(i); /* accelerate */

			drift(i);
		}
}



rot_accel(i)
/* Rotate and accelerate */
word	i;
{
	register	struct	sh_ship		*shptr;
    word		t1;
	double		r_times_t, a_div_r, a_div_rr, cos_r_times_t;
	double		sin_r_times_t, xn, yn;

	shptr = &ship[i];

	t1 = min(shptr->sh_rotate_timeleft, t);
	t1 = min(shptr->sh_thrust_timeleft, t1);	
	r_times_t = shptr->sh_rotate * t1;
	a_div_rr = (a_div_r = shptr->sh_thrust / shptr->sh_rotate) / shptr->sh_rotate;
	cos_r_times_t = 1.0 - cos(r_times_t);
	sin_r_times_t = sin(r_times_t);
	
	/* compute new displacement (rot and accel) */
	xn = a_div_rr * cos_r_times_t;
	yn = a_div_rr * (r_times_t - sin_r_times_t);

	shptr->sh_disp_i += shptr->sh_vel_i * t1 + 
		shptr->sh_bow_i * xn - shptr->sh_bow_j * yn;

	shptr->sh_disp_j += shptr->sh_vel_j * t1 +
		shptr->sh_bow_j * xn + shptr->sh_bow_i * yn;

	/* compute new velocity (rot and accel) */
	xn = a_div_r * sin_r_times_t;
	yn = a_div_r * cos_r_times_t;
	
	shptr->sh_vel_i += shptr->sh_bow_i * xn - shptr->sh_bow_j * yn;
	shptr->sh_vel_j += shptr->sh_bow_j * xn + shptr->sh_bow_i * yn;

	/* compute new bow position */
	cos_r_times_t = cos(r_times_t);

	xn = cos_r_times_t * shptr->sh_bow_i -
		sin_r_times_t * shptr->sh_bow_j;

	shptr->sh_bow_j = sin_r_times_t * shptr->sh_bow_i +
		cos_r_times_t * shptr->sh_bow_j;

	shptr->sh_bow_i = xn;

	/* update time variables */
	t -= t1;

	if ((shptr->sh_thrust_timeleft -= t1) <= 0)  { /* if stopped accelerating */
		shptr->sh_thrust_timeleft = 0;
		shptr->sh_thrust = 0.0;
	}

	if ((shptr->sh_rotate_timeleft -= t1) <= 0) { /* if stopped rotating */
		shptr->sh_rotate_timeleft = 0;
		shptr->sh_rotate = 0.0;
	}
}



rot(i)
/* Rotate */
word	i;
{
	register	struct	sh_ship		*shptr;
    word		t1;
	double		r_times_t, sin_r_times_t, cos_r_times_t;
	double		xn;

	shptr = &ship[i];

	t1 = min(shptr->sh_rotate_timeleft, t);
	sin_r_times_t = sin(r_times_t = shptr->sh_rotate * t1);

	xn = (cos_r_times_t = cos(r_times_t)) * shptr->sh_bow_i
		- sin_r_times_t * shptr->sh_bow_j;

	shptr->sh_bow_j = sin_r_times_t * shptr->sh_bow_i + cos_r_times_t
		* shptr->sh_bow_j;
	
	shptr->sh_bow_i = xn;

	t -= t1;	

	if ((shptr->sh_rotate_timeleft -= t1) <= 0) { /* if stopped rotating */
		shptr->sh_rotate_timeleft = 0;
		shptr->sh_rotate = 0.0;
	}
}



accel(i)
/* accelerate */
word	i;
{
	register	struct	sh_ship		*shptr;
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
			? -1 : 1);
		shptr->sh_thrust_timeleft = 1;
		shptr->sh_heat = 7999;
		shptr->sh_engine_blown = TRUE;
		shptr->sh_engine_wait = 60;
		shptr->sh_repair_wait = 40;
		mwrite((1 << i),"\007Bang, engines blew up!  Impulse power only.\n");
	}

	if (shptr->sh_engine_blown) { /* repair engine */
		if (shptr->sh_heat < 2000)
			shptr->sh_engine_wait -= t;
		else		
			shptr->sh_engine_wait = 60;

		if (shptr->sh_engine_wait <= 0) { /* if engine repaired */
			shptr->sh_engine_wait = 0;
			shptr->sh_engine_blown = FALSE;
			mwrite((1 << i),"\nEngine repaired.\n");
		}
	}

	shptr->sh_missle1_wait = max(0, shptr->sh_missle1_wait - t);
	shptr->sh_missle2_wait = max(0, shptr->sh_missle2_wait - t);
	shptr->sh_laser_wait = max(0, shptr->sh_laser_wait - t);
}
