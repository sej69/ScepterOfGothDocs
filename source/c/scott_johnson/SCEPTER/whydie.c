struct	ob_object	*object;
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
					count == 1 ? ".\n" : (count  > 2 ? ", " : " and "));
				--count;
			}
	}

	count = 0;	object = room[rm].ro_rmobjecttail;
	while (object != NULL) {
		if (object->ob_invisible == FALSE || user->us_ssj)
			++count;
		object = object->ob_next;
	}

	monster = room[rm].ro_rmmonstertail;			
	while (monster != NULL) {
		if (monster->mo_top && (monster->mo_invisible == FALSE || user->us_ssj))
			++count;
		monster = monster->mo_next;
	}

	if (count > 0) {
		mwrite(My_mask, "You see ");
		object = room[rm].ro_rmobjecttail;	
		monster = room[rm].ro_rmmonstertail;
		i = 0;
		while (i < count && monster != NULL) {
			if (monster->mo_top && (monster->mo_invisible == FALSE || user->us_ssj))
				if (monster->mo_num == 1) {
					++i;
					mwrite(My_mask, "a %s%s", monster->mo_name,
						monster->mo_invisible && user->us_ssj ? " (inv)" : "");
					punctuate(i, count);
				}
				else {
					++i;
					mwrite(My_mask, "%s %ss%s", pn(monster->mo_num),
						monster->mo_name,
						monster->mo_invisible && user->us_ssj ? " (i