/*		kill - kill player and suspend his file.
		84/08/08.  A. E. Klietz.
*/


#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

extern	word	nusers;

kill()
{
	char	nam[BUFLEN];

	deleteuser(user);
	mwrite(My_mask, desc(2738));
	/* Player file space released.\n */

	strncpy(nam, user->us_name, 12);
	user->us_name[0] = '\0';
	writeplayer(user, nam, genuser[user->us_trm].gn_userindex, TRUE);
	--nusers;
}
