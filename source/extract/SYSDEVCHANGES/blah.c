timeleft;
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
					mwrite(My_mask,"%d - no such s