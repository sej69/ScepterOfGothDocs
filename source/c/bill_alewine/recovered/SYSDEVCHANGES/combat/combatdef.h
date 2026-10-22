/*		Combatdef.h - Combat definitions.
		84/05/06.	A. E. Klietz
*/

#include <math.h>
extern	double	mag();
extern	double	azmuth();

#define MAX_SHIPS	MAX_PORTS

#define minf(a,b)  ((a) < (b) ? (a) : (b))
#define maxf(a,b)  ((a) > (b) ? (a) : (b))

enum en_entry {US_ENTERNAME, US_COMMAND};

struct		us_user {
	word		us_entry;
	boolean		us_brief;		 /* TRUE if brief mode */
	byte		us_vector;       /* -1 for no vector */
	longword	us_display_range;
};

extern	unsigned  All_mask, Staff_mask, Allbutme_mask, My_mask;
extern	struct	gn_genuser	*user, genuser[MAX_PORTS];

extern	word	My_port, My_ship;

extern	struct	us_user		*user, user_array[MAX_PORTS];
