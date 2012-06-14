#ifndef FS_H
#define FS_H

#define FS_ATTRIBUTE_READ_WRITE 0x00
#define FS_ATTRIBUTE_READ_ONLY  0x01
#define FS_ATTRIBUTE_HIDE       0x02
#define FS_ATTRIBUTE_SYS        0x04
#define FS_ATTRIBUTE_VOLUME     0x08
#define FS_ATTRIBUTE_FOLD       0x10
#define FS_ATTRIBUTE_RECORD     0x20



struct inode 
	// This Struct Used by Directory
{
	int               i_dev;
	int               i_isdir;
	unsigned short    i_fs_type;
	unsigned long     i_number;
	unsigned long     i_father;
	unsigned int      i_mode; ///is file or dir
	unsigned short	  i_links_count;	/////////// Links count 
	unsigned short    i_count;
	unsigned short    i_size;

	unsigned long	    i_blocks;	/////////// 块数Blocks count 
	unsigned long	    ext2_block[15];/////////// Pointers to EXT2 blocks 
} ;

#define NULL_INODE (struct inode *) 0


typedef struct
{
	unsigned int         f_sys_nr;
	unsigned int         f_mode; ///读还是写
	unsigned long        f_size; ///读还是写
	unsigned long        start_blk;
	unsigned long        father_blk;
	unsigned long        current_blk;
	unsigned long        current_pos;
	unsigned long        current_hasbuf;
}FILE;

#define NULL_FCB  (FILE *) 0

struct super_block {
	int s_dev;
	unsigned char s_count;
	unsigned char s_dev_name;
	unsigned int  s_start_sector;
	unsigned long s_blocksize;
	unsigned short s_flag;
	unsigned char s_rd_only;
	struct inode* root;
	struct fs_operations *ops;	// file operations
};



#endif
