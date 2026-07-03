onsters and objects that are not permanent */
word	rm;
{
	register	struct	ob_object	*obj;
	struct		ob_object	*obj2;
	register	struct	mo_monster	*mon;
	struct		mo_monster	*mon2;

	for (obj = room[rm].ro_rmobjecttail; obj != NULL; )
		if (!obj->ob_permanent) {		
			if (deleteobject(obj, room[rm].ro_rmobjecttail))
				room[rm].ro_rmobjecttail = room[rm].ro_rmobjecttail->ob_next;
			obj2 = obj->ob_next;
			odestroy(obj);
			obj = obj2;
		}
		else
			obj = obj->ob_next;
		
	for (mon = room[rm].ro_rmmonstertail; mon != NULL; )
		if (!mon->mo_permanent) {
			mon2 = mon->mo_next;
			deletemonster(mon, rm);
			destroy(mon);
			mon = mon2;
		}
		else {
			mon->mo_hits = mon->mo_maxhits;
			mon = mon->mo_next;
		}
}


deleteplayer(pt, rm)
struct	us_user	*pt;
word	rm;
{