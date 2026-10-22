/*	hashpassword 
	83/10/21.	A. E. Klietz.

*/

hashpassword(pw,hashpw)
/* Procedure hashpw - hash password into gibberish. */
char	*pw, *hashpw;
{
	
	int	seed = 0xc73f;

	while (*hashpw++ = *pw++ * seed)
		;
}
