
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
	shptr->sh_vel_j += shptr->sh_bow_j * xn + shpt