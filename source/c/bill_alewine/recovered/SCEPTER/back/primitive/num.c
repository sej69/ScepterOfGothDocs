/*		num - print numbers.
		84/06/20. A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/monster.h"

static	char	*ordinal[] = {
"zeroth",	"first", 	"second",	"third",	"fourth",	"fifth",	"sixth",
"seventh",	"eighth",	"ninth",	"tenth",	"eleventh",	"twelfth",
"thirteenth",			"fourteenth",			"fifteenth",
"sixteenth",			"seventeenth",			"eighteenth",
"ninteenth",			"twentieth" };


static	char	*numeral[] = {
"zero",		"one",		"two",		"three",	"four",		"five",
"six",		"seven",	"eight",	"nine",		"ten",		"eleven",
"twelve",	"thirteen",	"fourteen",	"fifteen",	"sixteen",	"seventeen",
"eighteen",	"nineteen",	"twenty" };

static	char	buf[85];
static	char	nums[12];
static	char	nm[10];
static	char	nth[10];

char	*pnth(n)
word	n;
{
	char	*suffix;

	if (n >= 0 && n <= 20)
		return(ordinal[n]);

	switch (n % 10) {
case 1: suffix = "st"; break;
case 2: suffix = "nd"; break;
case 3: suffix = "rd"; break;
default: suffix = "th"; break;
	}

	tsprintf(nth, "%.7d%.2s", n, suffix);
	return(nth);
}



char	*pn(n)
word	n;
{
	
	if (n >= 0 && n <= 20)
		return(numeral[n]);

	tsprintf(nm, "%.9d", n);
	return(nm);
}


char	*pm(mon)
struct	mo_monster	*mon;
{
	boolean		single;

	single = mon->mo_top && mon->mo_num == 1;
	if (!single)
		tsprintf(nums, " #%.7d", mon->mo_num);

	tsprintf(buf, "%.4s%.70s%.10s",
		(single ? "the " : ""),
		mon->mo_name,
		(single ? "" : nums));
	
	return(buf);
}
