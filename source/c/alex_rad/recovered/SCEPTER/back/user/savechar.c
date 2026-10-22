/*		savechar.c - save character.
		84/08/08.	A. E. Klietz

		Copyright (c) 1984 by Alan E. Klietz.    All Rights Reserved.
*/
#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

savechar()
{
	register	struct	us_user	*usr;

	usr	= validusr(user);
	writeplayer(usr, usr->us_name, genuser[usr->us_trm].gn_userindex, FALSE);
}
