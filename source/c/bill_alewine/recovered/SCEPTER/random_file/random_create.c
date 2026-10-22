/*		random_create - Create a random file.
		84/04/21.     A. E. Klietz.
*/

#include "/std/def.h"
#include "random_file.h"


word random_create(filename, initsize, maxcachesize)
/* create a random file starting with *size* words.  Must be
   at least 1024. */
char	*filename;
word	initsize, maxcachesize;
{
	struct	rb_root_block	root_block;
	struct	n_node			node;
	char *zeros;
	FILE	*fp;

	if (initsize < 1024)
		return(0);

	if (maxcachesize < 0 || maxcachesize > 16384)
		return(0);

	fdelete(filename);
	if ((fp = fopen(filename, "wv")) == 0)
		return(0);

	zero(&root_block, sizeof(root_block));
	root_block.rb_corrupt = 0;	
	root_block.rb_checkword = N_CHECK;
	root_block.rb_filesize = initsize;
	root_block.rb_maxcachesize = maxcachesize;

	fput(&root_block, sizeof(root_block), fp);

	initsize -= sizeof(root_block);

	zero(&node, sizeof(node));
	node.n_checkword = N_CHECK;
	node.n_allocflag = 0;
	node.n_index = 0;
	node.n_size = initsize;
	node.n_data_size = 0;
	
	fput(&node, sizeof(node), fp);
	
	zeros = alloc(initsize - sizeof(node));	
	fput(zeros, initsize - sizeof(node), fp);
	free(zeros);
	fclose(fp);

	return(random_open(filename));
}
