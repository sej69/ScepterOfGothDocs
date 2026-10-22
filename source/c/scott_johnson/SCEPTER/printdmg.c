tobj(object, FALSE));
			punctuate(i, count);
			object = object->ob_next;
		}
		mwrite(My_mask, ".\n\n");
	}
	else
		mwrite(My_mask, desc(841));
		/* Nothing at all.\n */
}	


roomdisplay(rm, brief)
word	rm;
boolean	brief;
{
	register	i, count;
	register	struct	ob_object	*object;
	register	struct	mo_monster	*monster;
	register	struct	us_user	*player;
	char	*tense[2];

	mwrite(My_mask, "You're %s", printdesc(room[rm].ro_desccode, 0,
		brief));
	if (user->us_ssj)
		mwrite(My_mask, " (%d)", w(rm));
	mwrite(My_mask, "\n");
	count = 0;
	for (i=0; i < 7; ++i)
		if (room[rm].ro_adjoin[i] != 0)
			++count;
	if (count > 0) {
		mwrite(My_mask,"Obvious exits are ");
		for (i=0; i < 7; ++i) 
			if (room[rm].ro_adjoin[i] != 0) {
				mwrite(My_mask, "%s%s", dirlist[i],
	