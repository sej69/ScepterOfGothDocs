/*		Shipdef.h - Spaceship definitions.
		84/05/06.	A. E. Klietz
*/


struct		sh_ship {
	float		sh_vel_i;
	float		sh_vel_j;
	float		sh_disp_i;
	float		sh_disp_j;
	double		sh_bow_i;		/* unit vector */
	double		sh_bow_j;		/* unit vector */
	float		sh_thrust;
	word		sh_thrust_timeleft;
	double		sh_rotate;		/* radians/sec */
	word		sh_rotate_timeleft;
	float		sh_ship_dmg;	/* percent */
	longword	sh_power;
	struct {
		float		sh_shield_dmg;  /* percent */
		byte		sh_pwr;	/* 0-25 */
		byte		sh_repair; /* percent repair effort */
	} sh_shield[2];
	word		sh_heat;
	boolean		sh_good_azmuth[MAX_SHIPS]; /* TRUE if azmuth up to date */
	double		sh_azmuth[MAX_SHIPS]; /* azmuth to other ships. */
	word		sh_engine_wait;
	boolean		sh_engine_blown; /* TRUE if engines blown up */
	boolean		sh_exist;	/* TRUE if ship exists, else FALSE */
	byte		sh_ship_repair;  /* shield repair + ship repair <= 100 */
	byte		sh_repair_wait;
	byte		sh_missle1_wait;
	byte		sh_missle2_wait;
	byte		sh_laser_wait;
	byte		sh_missles;	
	byte		sh_kills;
	byte		sh_lasthit;    /* ship that last hit our ship, -1 for none */
};



extern		struct	sh_ship		ship[MAX_PORTS];
