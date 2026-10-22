/*		update - update the room files.
		84/06/21.	A. E. Klietz.
*/

#define NONSTANDARD
#include "/std/def.h"
#include "/def/multidef.h"
#include "^^def/scepter.h"
#include "^^def/user.h"

#define TASKCLOSED_TXT	200
#define	NUMRUN_TXT	204
#define MSPEED_TXT 206

/* The following definitions are duplicated in buy.c and whydie.c */

#define NUM_OBITS   3   /* Number of obituaries in the paper */
#define OLD_OBIT	251 /* Base index for yesterday's obits */
#define	NEW_OBIT	261 /* Base index for today's obits */
#define MONTHDAY	259 /* Day-of-month for yesterday's obits */


int		lencounter, objlistlen;

int		(*encounterindex)[6];
int		(*objindex)[6];

word	obit_lvl[NUM_OBITS];
word	mspeed;

extern	word	slottbl[MAX_PORTS + 1];
extern	char	taskclosed;
extern	word	numrun;
extern	TI_TIME	lastupdate;
extern	unsigned	elist;

static	word	monthday;

update()
{
	register	i;
	char	buf[BUFLEN];

	for (i=0; i < MAX_PORTS + 1; ++i)
		if (slottbl[i] > -1)
			writeseg(i, slottbl[i], FALSE);

	tsprintf(buf, "%c", taskclosed);
	writedesc(TASKCLOSED_TXT, buf);

	tsprintf(buf, "%d", mspeed);
	writedesc(MSPEED_TXT, buf);

	random_write(elist, 1, encounterindex, lencounter * 6 * sizeof(word));
	random_write(elist, 2, objindex, objlistlen * 6 * sizeof(word));
}



clean_update()
/* Update and free EVERYTHING to clean up holes in memory. */
{
	register	i,j;
	char	buf[BUFLEN], nametable[MAX_PORTS][12];
	unsigned	trmtable[MAX_PORTS], suspended_trm;
	register	struct	us_user		*tmpuser;

	i = -1;
	suspended_trm = user->us_trm;
	user = usertail;
	while (user != NULL) {
		if (user->us_status != SNORMAL) {
			user = user->us_nextuser;
			continue;
		}

		++i;
		My_mask = 1 << user->us_trm;
		user->us_usweap = NULL;
		stopusing(user, user->us_usarm);
		stopusing(user, user->us_usshield);
		tmpuser = user->us_nextuser;
		deleteuser(user, usertail);
		strncpy(nametable[i], user->us_name, 12);
		trmtable[i] = user->us_trm;
		if (writeplayer(user, user->us_name, genuser[user->us_trm].gn_userindex, FALSE))
			writeusr(user);
		else
			--i;
		user = tmpuser;
	}

	for (j=0; j < MAX_PORTS + 1; ++j)
		if (slottbl[j] > -1) {
			writeseg(j, slottbl[j], TRUE);
			slottbl[j] = -1;
		}

/* Everything is released.  Now read it all back in. */

	for (j=0; j <= i; ++j) {
		user = alloc(sizeof(struct us_user));
		user->us_checkword = CHECKWORD;
		readplayer(user, nametable[j], genuser[trmtable[j]].gn_userindex);
        if (user->us_name[0] == '\0') {
			error_handler(0);
			crash(desc(543));
			/* Failed to read back in user from update!\n */
		}

		user->us_trm = trmtable[j];
		user->us_entry = XCMD;
		user->us_status = SNORMAL;
		user->us_nextuser = usertail;
		usertail = user;
		placeplayer(user, user->us_rmcode);
	}
		
	user = usertail;
	while (user != NULL)
		if (user->us_trm == suspended_trm)
			break;
		else
			user = user->us_nextuser;

	if (user == NULL)
		crash(545);
		/* Sce545 - Could not restart suspended user!\n */

	My_mask = 1 << user->us_trm;

	tsprintf(buf, "%c", taskclosed);
	writedesc(TASKCLOSED_TXT, buf);

	tsprintf(buf, "%d", mspeed);
	writedesc(MSPEED_TXT, buf);

	random_write(elist, 1, encounterindex, lencounter * 6 * sizeof(word));
	random_write(elist, 2, objindex, objlistlen * 6 * sizeof(word));
}



break_trap()
{
	unbreakable();

	crash("Sce000 - Break signal received.\n");
}



error_trap()
/* Called on error.   Return 0 to abort, else error recovered. */
{
	char	buf[BUFLEN];

	error_handler(0);  /* Turn off further error handling */
	update();
	tsprintf(buf, "%d", numrun+1);
	writedesc(NUMRUN_TXT, buf);
	tsprintf(buf, "%d", monthday);
	writedesc(MONTHDAY, buf);

	if (user != NULL && user->us_checkword == CHECKWORD) 
		errlog("Player %s caused the pta with the \"%s\" cmd in room %d.\n",
			user->us_name, user->us_lastcmd, w(user->us_rmcode));
	return(0);  /* Crash */
}



do_initialize()
{
	register	i;
	struct		bt_broken_time	*broken_time;
	char		*string;

	if ((elist = random_open("/data/scepter/elist")) == 0)
		crash(desc(1252));
		/* Sce1252 - Unable to open /data/scepter/elist!\n */

	if ((lencounter = random_allocread(elist, 1, &encounterindex, 6000))
			== 0) {
		lencounter = 1;
		encounterindex = alloc(sizeof(int) * 6);
	}
	else
		lencounter /= sizeof(int) * 6;

	if ((objlistlen = random_allocread(elist, 2, &objindex, 6000)) == 0) {
		objlistlen = 1;
		objindex = alloc(sizeof(int) * 6);
	}
	else
		objlistlen /= sizeof(int) * 6;
	
	for (i=0; i < MAX_PORTS + 1; ++i)
		slottbl[i] = -1;

	tsscanf(desc(MONTHDAY), "%d", &monthday); 
	tsscanf(desc(TASKCLOSED_TXT), "%c", &taskclosed);
	tsscanf(desc(NUMRUN_TXT), "%d", &numrun);
	mspeed = 6; /* default */
	tsscanf(desc(MSPEED_TXT), "%d", &mspeed);
	time(&lastupdate);

	broken_time = bt_time(&lastupdate);
	if (monthday != broken_time->bt_day) { /* If new day */
		monthday = broken_time->bt_day;
		for (i=0; i < NUM_OBITS; ++i) {
			string = desc(i + NEW_OBIT);
			writedesc(i + OLD_OBIT, string);
		}
	}
	else 
		for (i=0; i < NUM_OBITS; ++i) {
			string = desc(i + NEW_OBIT);
			tsscanf(string, "%d", &obit_lvl[i]);
		}
	error_handler(error_trap);
	exc_handler(EXC_BREAK, 0, break_trap);
}
