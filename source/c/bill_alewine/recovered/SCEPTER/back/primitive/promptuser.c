/*		promptuser - prompt the user
		84/06/04.	A. E. Klietz.
*/

#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

extern	boolean	noprompt;

promptuser()
/* Issue a prompt based on the user->us_entry parameter */
{
	register	struct	us_user	*usr;

	usr = user;
	usr->us_encountstop = FALSE;
	switch (usr->us_entry) {

case XNEWCLASS: mwrite(My_mask,"Class");	
				break;

case XSTATS:	mwrite(My_mask, "Stats");
				break;

case XSKILL:	mwrite(My_mask, "Skill");
				break;

case XSEX: 		mwrite(My_mask, "%s", desc(500));
				/* Finally, is your character a (M)ale or a (F)emale */
				break;

case XCMD:
				if (usr->us_brief) 
					mwrite(My_mask, "Do");
				else
					mwrite(My_mask, "Action");
				break;

case XEDIT:
				if (usr->us_brief)
					mwrite(My_mask, "Ed");
				else
					mwrite(My_mask, "Edit command");
				break;

case XNOTICE:   mwrite(My_mask, "Enter notice");
				break;

case XDEAD:
{	
		register	i;

		for (i=501; i <= 505; ++i)
			mwrite(My_mask, fmt, desc(i));
				/* Congratulations!  Your death has made the newspapers! */
				/* In one line, try to explain what killed you and how.  Use */
				/* words like "He" and "She" instead of "I". */
				/* Example: "His weapon broke, and before he knew it the vampire */
				/* drained his life away.." */
			break;
}

case XSPELL:
	mwrite(My_mask, "Say chant");
	break;

case XNEWS:
	mwrite(My_mask, "Enter news");
	break;

case XSELL:
case XREPAIR:
	mwrite(My_mask, "Yes/no");
	break;

case XWISH:
	mwrite(My_mask, "Enter wish");
	break;

case XSURE:
	mwrite(My_mask, "Are you sure");
	break;

case XPARLEY:
	mwrite(My_mask, "Would you like to have it, Effiendi");
	break;

default:
	errlog(fmt, desc(506));
	/* sce506 - Unknown prompt. */
	break;
}

}
