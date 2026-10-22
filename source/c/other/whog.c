/*	whog - who (gambit version)
	83/10/30.	A. E. Klietz.

*/
#include "/def/aekdef.h"
#include <stdio.h>
#include "/def/globdef.h"
#include "/def/appdef.h"

char    *ap_list[] =  { 
		"<login>",
		"Menu",					   
		"Scepter",  
		"Talk",     
		"Mail/Topics", 
		"Diplomacy",   
		"<system>",    
};

struct	gl_user_datum		*user_data_ptr;
struct	gl_system_datum		*system_data_ptr;

main()
{
		BUFTYPE		word;
		int			i;
		unsigned	bindate[2];

    	set_glob();
		
		print("Name          Time on   tty Port    Act Indx Tid  Task\n");         

		for (i=0; i < NUMDEVICES; i++) 
			if (@&user_data_ptr[i].gl_userindex > 0) {
                strcpy_star_from_at(word, user_data_ptr[i].gl_username);
				print("%-11s",word);
				print("%c ",(@&user_data_ptr[i].gl_useracct == 255) ? '*' : ' ');
				bindate[0] = @&user_data_ptr[i].gl_logintime[0];				
				bindate[1] = @&user_data_ptr[i].gl_logintime[1];
				print("%-11s ",timeconv(bindate));
				print("%2d ", i);
				strcpy_star_from_at(word, user_data_ptr[i].gl_ttyname);
				print("%-7s ",word);
				print("%03d-%04d ",@&user_data_ptr[i].gl_useracct,
					@&user_data_ptr[i].gl_userindex);
				print("%04x ",tty_tid(i));
				print("%s\n",ap_list[@&user_data_ptr[i].gl_apnum]);
			}
}   
