#ifndef MSDOS_H
#define MSDOS_H

#include "CTYPE.h"

#define FAT12_MAX    4078 // 12 位 FAT的最大串号 
#define FREE_FAT12   0x0

#define FAT12_MAGIC	 4085
#define FAT16_MAGIC	 (( unsigned ) 65526l )
#define FAT32_MAGIC	 268435455l

#define BAD_FAT12    0xff7
#define BAD_FAT16    0xfff7
#define BAD_FAT32    0xffffff7

#define EOF_FAT12    0xff8		// standard EOF 
#define EOF_FAT16    0xfff8
#define EOF_FAT32    0xffffff8
#define FILE_NOT_FOUND  0xffffffff
#define FAT12_ROOT	 1

#define FAT12   0x12
#define FAT16   0x0e
#define FAT32   0x0b
#define EXT2    0x83
#define UJFS    0x82

#define PACKED __attribute__((packed))


typedef struct
{
	signed char	jmp_boot[3];	// Boot strap short or near jump 
	signed char	oem[8];	      // Name - can be used to special casepartition manager volumes 
	u8_t	sector_size[2];	    // bytes per logical sector 
	u8_t	cluster_size;	      // sectors/cluster 
	u16_t	reserved;	          // reserved sectors 
	u8_t	fats;		            // number of FATs 
	u8_t	dir_entries[2];	    // root directory entries 
	u8_t	sectors[2];	        // number of sectors 
	u8_t	media_descriptor;		// media code (unused) 
	u16_t	fat16_length;	      // sectors/FAT 
	u16_t	sec_per_track;	    // sectors per track 
	u16_t	heads;		          // number of heads 
	u32_t	hidden;		          // hidden sectors (unused) 
	u32_t	total_sectors;	    // number of sectors (if sectors == 0) 

	// The following fields are only used by FAT32 
	u32_t	fat32_length;	      // sectors/FAT 
	u16_t	flags;		          // bit 8: fat mirroring, low 4: active fat 
	u8_t	version[2];	        // major, minor filesystem version 
	u32_t	fat32_root_cluster;	// first cluster in root directory 
	u16_t	info_sector;	      // filesystem info sector 
	u16_t	backup_boot;	      // backup boot sector 
	u16_t	unused[6];	        // Unused 
}MSDOS_SUPER;

typedef struct
{
	 u16_t flag;
	 u16_t sector_size;
	 u8_t  cluster_size;
	 u32_t blk_size;
	 u32_t fat_size;
	 u16_t dir_entries;
	 u16_t fat_entries;
	 u32_t fat_base;
	 u32_t fat_root;
	 u32_t fs_base;
	 u32_t blk_base ;  //////数据区开始
	 MSDOS_SUPER dos_sp;

}MSDOS_INFO;

typedef struct {
  unsigned sec:5;
  unsigned min:6;
  unsigned hour:5;
}__attribute__((packed)) DOS_TIME;


typedef struct  {
  unsigned day_of_month:5;
  unsigned month:4;
  unsigned year:7; /* since 1980 */
}__attribute__((packed)) DOS_DATE;

typedef struct
{
	u8_t file_name[8];
	u8_t ext_name[3];	// name and extension 
	u8_t attribute;		// attribute bits 

	u8_t    lcase;		// Case for base and extension 
	u8_t	  ctime_ms;	// Creation time, milliseconds 
	u16_t	  ctime;		// Creation time 
	u16_t	  cdate;		// Creation date 
	u16_t	  adate;		// Last access date 
	u16_t   first_high;	// High 16 bits of cluster in FAT32 

	DOS_TIME	time;
	DOS_DATE date;
	u16_t first_cluster;// first cluster 
	u32_t	file_size;		// file size (in bytes) 
}__attribute__((packed)) MSDOS_DIR;

typedef struct __fat
{
	unsigned fat_item:12;
}PACKED FAT12_TAG;

//typedef unsigned short FAT12_TAG;

#define MSDOS_READONLY      1                  // 只读
#define MSDOS_HIDDEN  2                        // 隐藏文件 
#define MSDOS_SYS_FILE     4                   // 系统文件 
#define MSDOS_VOLUME  8                        // 卷标 
#define MSDOS_FLOD     16                      // 目录 
#define MSDOS_ARCH    32                       // 存档文件 

#define MSDOS_ISVOLUME(attribute)		(((attribute) &0x20) && ((attribute) & 0x08))
#define MSDOS_ISDIR(attribute)		  (((attribute) &0x10) && !((attribute) & 0x08))
#define MSDOS_ISREG(attribute)		  (!((attribute) & 0x08) && !((attribute) &0x10))

void  Fat12_Init(void);
//unsigned long Fat12_LoadFile(char* filename,void **ptr);
//void  Fat12_LoadProgram(char* filename,void* buf);
FILE* Fat12_OpenFile(char* filename);
int Fat12_Read(FILE*fp, void *buff, unsigned long __sb);

int Fat32_Init(void);
int Fat32_Read(FILE*fp, void*buff, int __sb);
FILE* Fat32_OpenFile(char* filename);
void Fat32_PrintRootDir(void);



#endif
