/*		ran - Random number generator.
		84/06/15.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"

word	ran(n)
/* Return a random number from 1 to n. */
word	n;
{
	double	rnd();

	return(((word) (rnd() * n)) + 1);	
}
