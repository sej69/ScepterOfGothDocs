/*		time - System time and date routines.
		83/09/25.	A. E. Klietz.

		dateconv(bindate) - converts the binary date array bindate[2]
into a character string of the format:

			"83 Sep 25"

		timeconv(bindate) - converts the binary date array bindate[2]
into a character string of the format:

			"11:34:45 pm"
	
		date() - returns the date in a string as above.

		time() - returns the time in a string as above.

		secdiff(bindate1, bindate2) - returns the difference in
seconds of bindate2 - bindate1.  If > 18 hours, then it returns
65535.

		mindiff(bindate1, bindate2) - returns the difference in
minutes of bindate2 - bindate1.  If > 22 days, then it returns
31680 minutes.

		count_days(bindate) - returns the number of days since Jan 1st,
1980.
*/

#asm "seg 2"  /* force array into segment 2 (constants) */
static char months[13][4] {
	"???", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

#asm "seg 2"
static int year_count[16] {
0, 366, 731, 1096, 1461, 1827, 2192, 2557, 2922, 3288, 3653, 4018, 4383, 
4749, 5114, 5479 };

#asm "seg 2"
static int day_count[2][12] {
0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };


char *dateconv(d)                                 
unsigned	d[]; /* binary date array [2] */
{                            

	static char datestr[12];

	strcpy(datestr,"           "); /* zero out old date. */
	tsprintf(datestr,"%u %s %u", (d[0] >> 12)+80, months[(d[0] >> 8) & 0xf],
		(d[0] >> 1) & 0x3f);
	
	return(datestr);
}

char *timeconv(d)
unsigned	d[]; /* binary date array [2] */
{
	unsigned hour;

	static char timestr[12];

	strcpy(timestr,"           "); /* zero out old time. */
	hour = d[1] / 3600;
	tsprintf(timestr, "%2u:%02u:%02u %s", hour == 0 ? 12 : hour,
		(d[1]%3600)/60, d[1]%60, d[0] & 01 ? "pm" : "am");

	return(timestr);
}



char *date()
{
	unsigned	bindate[2];

	get_date(bindate);
	return(dateconv(bindate));
}



char *time()
{
	unsigned	bindate[2];

	get_date(bindate);
	return(timeconv(bindate));
}



unsigned secdiff(bindate1, bindate2)
/* Procedure secdiff - return difference in seconds bindate2 - bindate1.
If the difference is more than 18.2 hours, then 65535 is returned. */
unsigned	bindate1[], bindate2[];
{
	unsigned	datediff, secs, secdiff;

	secs = bindate2[1] - bindate1[1];
	if (bindate1[0] == bindate2[0])
		return(secs);
	else {
		datediff = count_days(bindate2) - count_days(bindate1);
        datediff = datediff * 2 + (bindate2[0] & 0x1) - (bindate1[0] & 0x1);
		secdiff = bindate2[1] - bindate1[1];
		secs = 43200 + secdiff;

/* datediff now has the difference in dates in twelve hour intervals. */

		if (datediff > 2 || datediff == 1 && secdiff >= 22336
			||	datediff == 2 && secs >= 22336)
	      	return(65535);
		else /* datediff must be 1 or 2. */
			return(secs + 43200 * (datediff - 1));
		}
}



unsigned mindiff(bindate1, bindate2)
/* Procedure mindiff - return the difference in minutes bindate2 - bindate1.
The difference is truncated; any remaining seconds are discarded.
If the difference is more than 22 days, then 22 days (31680 min) is
returned. */
unsigned	bindate1[], bindate2[];
{
	unsigned datediff, min, sec1, sec2;
	
	sec1 = bindate1[1];
	sec2 = bindate2[1];
	min = (sec2 - sec1) / 60;
	if (bindate1[0] == bindate2[0])
		return(min);
	else {
		datediff = count_days(bindate2) - count_days(bindate1);
		datediff = datediff * 2 + (bindate2[0] & 0x1) - (bindate1[0] & 0x1);
		min = (43200 + sec2 - sec1) / 60;

		if (datediff > 44)	/* 22 days. */
			return(31680);  /* 22 days max. */
		else
			return(min + (datediff - 1) * 720);
	}
}



						 
int count_days(bindate)
/* Procedure day_count - count the number of days since Jan 1st, 1980. */
unsigned	bindate[];
{
	int		month, day, year;

	day = (bindate[0] >> 1) & 0x3F;
	month = (bindate[0] >> 8) & 0xF;
	year = bindate[0] >> 12;

	return(year_count[year] + day_count[year % 4 == 0][month] + day);
}
