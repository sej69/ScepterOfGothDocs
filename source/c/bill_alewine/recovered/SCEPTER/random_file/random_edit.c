/*		random_edit - edit a random file.
		84/06/06	A. E. Klietz.

		Copyright (c) 1984.  Alan E. Klietz.  
*/
#include "/std/def.h"

main()
{
	char	buffer[256], wrd[256], wrd2[256], fbuf[256];
	char	*data;
	register	i,	fp, j, k;


	unbreakable();
	setup("random_test");

	set_handler();
	setexit();
	for (;;) {
		breakable();
		print("random_edit? ");
		if (readline(buffer) == 0)
			continue;
		reset_parse();
		parse(buffer, wrd);
		if (subset(wrd, "end"))
			exit(0);
		if (subset(wrd, "read")) {
			parse(buffer, wrd);
			i = atoi(wrd);
			if (i == 0) {
				print("Missing record number.\n");
				continue;
			}
			print("Reading record %d.\n",i);
			if (random_read(fp, i, fbuf, 255) <= 0)			
				print("Read failed.\n");
			else {
				fbuf[255] = '\0';
				print("%s\n", fbuf);
			}
			continue;
		}
        if (subset(wrd, "write")) {
			parse(buffer, wrd);
			i = atoi(wrd);
			if (i == 0) {
				print("Missing record number.\n");
				continue;
			}
			print("Enter text for record %d\n? ", i);
			reset_parse();
			readline_wide(buffer, 255);
			j = strlen(buffer) + 1;
			for (k=0; k < j-1; ++k)
				if (buffer[k] == '\\' && buffer[k+1] == 'n') {
					buffer[k] = '\n';
					strcpy(&buffer[k+1], &buffer[k+2]);
					--j;
				}
					
			if (j <= 1) {
				j = 0;
				print("Erasing line.. ");
			}
			if (random_write(fp, i, buffer, j) != j)
				print("Write failed.\n");
			else
				print("Write ok.\n");
			continue;
		}
		if (subset(wrd, "open")) {
			if (parse(buffer, wrd) <= 0) {
				print("Missing name of file to open.\n");
				continue;
			}
			if ((fp = random_open(wrd)) == 0)
				print("open failed.\n");
			else
				print("file opened.\n");
			continue;
		}
		if (subset(wrd, "close")) {
			random_close(fp);
			print("Random file closed.\n");
			continue;
		}
		if (subset(wrd, "create")) {
			parse(buffer, wrd);
			parse(buffer, wrd2);
			i = atoi(wrd2);
			parse(buffer, wrd2);
			j = atoi(wrd2);
			if ((fp = random_create(wrd, i, j)) == 0)
				print("create failed.\n");
			else
				print("%s created.\n", wrd);
			continue;
		}
		if (subset(wrd, "compact")) {
			random_compact(fp);
			print("File compacted.\n");
			continue;
		}
        if (subset(wrd, "list")) {
           parse(buffer,wrd);
           i = atoi(wrd);
              if(i == 0) {
                 print ("Missing list start.\n");
                 continue;
              }
           parse(buffer,wrd);
           j = atoi(wrd);
              if(j == 0 || j < i) {
                 print ("Bad list end.\n");
                 continue;
              }
           for (;i < j+1;++i)
              if(random_read(fp,i,fbuf,255)>0)
                 print ("%d: %s\n",i,fbuf);
         }
		if (subset(wrd, "help")) {
			print("Edit commands are:\n");
            print ("Close Compact End Help List Open Read Write. \n\n");
			continue;
		}
	}
}
