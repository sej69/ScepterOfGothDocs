/*		shoot - combat shoot'em up processor.
		84/05/06.	A. E. Klietz
*/

#define NONSTANDARD
#include "/std/def.h"
#include "/def/multidef.h"
#include "combatdef.h"
#include "shipdef.h"

	struct	sh_ship		*shptr;
	char	wrd[BUFLEN];	
	double	width;
	word	shld, i, beam, sh, dmg1, dmg2;
	longword	dmg;
	boolean	hit = FALSE;
main(buffer)
char	*buffer;
/* shoot laser */
{
		beam = 10;
		width = 1.0;
		shld = 25;

				dmg = (beam/width) * 24 * (33 - shld);
	printf("%ld.\n", dmg);
}
