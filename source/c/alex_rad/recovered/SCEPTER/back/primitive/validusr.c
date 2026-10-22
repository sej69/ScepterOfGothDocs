/*		validusr - validate user record
		84/06/15.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"


struct	us_user	*validusr(usr)
struct	us_user	*usr;
{
	if (usr == NULL)
		crash(desc(1010));
		/* Sce1010 - NULL user pointer!\n */

	if (usr->us_checkword != CHECKWORD)
		crash(desc(1011));
		/* Sce1011 - Bad checkword on user pointer!\n */

	return(usr);
}
