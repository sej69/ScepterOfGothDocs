

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
				shptr->sh_good_azm