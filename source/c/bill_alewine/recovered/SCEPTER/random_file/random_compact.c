/*		random_compact - Compact a random file.
		84/04/22.     A. E. Klietz.
*/

#include "/std/def.h"
#include "random_file.h"

random_compact(indexfp)
/* close file, compact, and reopen with same fp. */
word indexfp;
{
	char	filename[BUFLEN], newname[BUFLEN];
	register	struct	rf_random_file	*filedesc;
	struct	rb_root_block 	root_block;
	struct	n_node			node;
	char	*data;
	FILE	*fp, *newfp;
	longword	filesize;

	if ((filedesc = get_rf_index(indexfp)) == NULL_RANDOM_FILE)
		return;

	strcpy(filename, filedesc->rf_name);
	random_close(indexfp);
	tsprintf(newname, "tmp%d", abs(getpid()));

	if ((fp = fopen(filename, "rv")) == 0) /* if cannot */
		crash("fatal:random_compact - cannot reopen %s.\n", filename);

	fdelete(newname);  /* erase any old version of file */
	if ((newfp = fopen(newname, "rwv")) == 0)
		crash("fatal:random_compact - cannot open %s.\n", newname);

	fget(&root_block, sizeof(root_block), fp);
	if (root_block.rb_checkword != N_CHECK) 	
		crash("fatal:random_compact - bad checkword on %s.\n", filename);

	fput(&root_block, sizeof(root_block), newfp);
	filesize = sizeof(root_block);

	tfprintf(stdout,"Active nodes: ");
	while (fget(&node, NODE_SIZE, fp) == NODE_SIZE) {
		if (node.n_checkword != N_CHECK) { /* if bad checkword */
			errlog("warn:random_compact - bad checkword at %d in %s.\n",
				(word) filesize, filename);
			continue;
		}
		tfprintf(stdout, "%d ",node.n_index);
		if (node.n_allocflag >= 0) /*if not allocated */
			fseek(fp, (longword) (node.n_size - NODE_SIZE), 1); /* skip */
		else { /* copy and compact */
			data = alloc(node.n_size - NODE_SIZE);
			fget(data, node.n_size - NODE_SIZE, fp);
			if ((node.n_size - node.n_data_size - NODE_SIZE) * 4 > 
					node.n_size) /* if more than 25 percent empty */
				node.n_size = node.n_data_size + NODE_SIZE +
					(node.n_data_size + NODE_SIZE) / 4;
				/* round down node to 25 percent empty */
			fput(&node, NODE_SIZE, newfp);
			fput(data, node.n_size - NODE_SIZE, newfp);
			free(data);
			filesize += node.n_size;
		}
	}
	rewind(newfp);	
	root_block.rb_filesize = filesize;
	fput(&root_block, sizeof(root_block), newfp);
	fclose(fp);
	fclose(newfp);
	call("/cmds/copy", newname, filename);
	fdelete(newname);
	random_fpopen(filedesc, filename);	
	tfprintf(stdout, "\n");
}
