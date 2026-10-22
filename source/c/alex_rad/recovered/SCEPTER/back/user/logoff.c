/*		Logoff - log off the user.
		84/06/08. A. E. Klietz.

		Copyright (c) 1984, Alan E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

extern	word	nusers;

logoff(usrptr)
struct	us_user		*usrptr;
{
	register	struct	us_user	*usr;
	register	kludge;

	usr = validusr(usrptr);
	
	if (usr->us_status != SLOGIN && usr->us_status != SINIT ||
			usr->us_entry == XDEAD) { 
		deleteuser(usr);
		if (My_mask == 0) { /* if hung up */
			if (usr->us_lvl >= 5)
				daymsg("Hup", "", 0);
	/* KLUDGE AROUND COMPILER BUG */
			kludge = usr->us_con;
			usr->us_con = max(3, kludge - 1);	
			/* Take away con as punishment for hangup */
			mwrite(Allbutme_mask,
				"### %s, the Coward, hung up %s phone\n",
				genuser[My_port].gn_name,
				usr->us_sex == MALE ? "his" : "her");
		}
		writeplayer(usr, usr->us_name, genuser[usr->us_trm].gn_userindex, TRUE); 
	}
	else {
		usr->us_rmcode = 0;
		deleteuser(usr);
		writeusr(usr);
	}

	--nusers;
}



off()
/* Kick off all users in preparation for a crash */
{
	user = usertail;

	while (user != NULL) {
		My_port = user->us_trm;
		My_mask = 1 << My_port;
		logoff(user);
		genuser[My_port].gn_logout = TRUE;
		user = usertail;
	}
}           



do_logout(port)
int		port;
{
	unsigned	mask;
	int			i;
	struct		gn_genuser		*gnptr;

	gnptr = &genuser[port];
	mask = 1 << port;
	My_port = port;
	gnptr->gn_ttynum = -1;
	gnptr->gn_usernum = 0;
	gnptr->gn_userindex = 0;
	gnptr->gn_useracct = 0;
	gnptr->gn_logout = FALSE;
	gnptr->gn_tid	= 0;
	gnptr->gn_waitmask = 0;
	gnptr->gn_name[0] = '\0';
	All_mask &= ~mask;
	Staff_mask &= ~mask;
}
