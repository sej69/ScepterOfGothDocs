cleanrm(rm);
	}
	else {
		for (; pt2 != NULL; pt2 = pt2->us_next)
			if (pt2->us_next == pt)
				break;

		if (pt2 == NULL)
			crash(desc(788));
			/* Sce788 - Deleted player not found!\n */
		else
			pt2->us_next = pt->us_next;
	}
	
	stopotheratk(pt, rm);