 disk_type;
	char disk_drv;
	unsigned blk_offset;
	unsigned num_sctrs;
	unsigned sctr_cyl;
	char sctr_trk;
	char sctr_base;
	unsigned ctl_addr;
	char ctl_int;
	char disk_filler[3];
	} ;

struct driver_entry {
	unsigned driver_seg;
	int (*disk_init) ();
	int (*blk_read) ();
	int (*blk_write) ();
	} ;

struct driver_msg {
	char mtype;
	char driver_type;
	struct driver_entry drv_ent;
	} ;

/*
 * define file status messages ("ored" with ERROR on return)
 */

#define UNABLE_TO_ACCESS	1
#define FILE_BUSY			2
#def