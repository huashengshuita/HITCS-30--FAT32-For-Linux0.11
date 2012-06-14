#ifndef GRUB_H
#define GRUB_H

#define CHECK_GRUB(flags,bit) ((flags)&(1<<(bit)))


#define MULTIBOOT_MEMORY	          0
#define MULTIBOOT_BOOT_DEVICE	      1
#define MULTIBOOT_COMMAND_LINE	    2
#define MULTIBOOT_MEM_MAP 	        6
#define MULTIBOOT_OS_NAME 	        9

#define MULTIBOOT_HEADER_FLAGS		0x00010003


typedef struct 
{
  unsigned long flags;
  /* Available memory from BIOS */
  unsigned long mem_lower;
  unsigned long mem_upper;
 
  /* "root" partition */
  unsigned char boot_device[4];
  
   /* Kernel command line */
  unsigned long cmdline;
  /* Boot-Module list */
  unsigned long mods_count; //MODULE
  unsigned long mods_addr;

      /*  Kernel symbol table info */
  unsigned long HeaderTable[4];

  /* Memory Mapping buffer */
  unsigned long mmap_length;
  unsigned long mmap_addr;

  /* Drive Info buffer */
  unsigned long drives_count;
  unsigned long drives_addr;

  /* ROM configuration table */
  unsigned long config_table;
  unsigned long boot_loader_name;
  
	  /*other information*/
  unsigned long apm_table;

  /* Video */
  unsigned long  vbe_control_info;
  unsigned long  vbe_mode_info	;
  unsigned short vbe_mode	;
  unsigned short vbe_interface_seg;
  unsigned short vbe_interface_off;
  unsigned short vbe_interface_len;
}GrubInfo;

int GetGrubVersion(GrubInfo* info);
int GetGrubBoot(GrubInfo* info, int* DevNo);
int GetGrubMemInfo(GrubInfo* info, unsigned long* lower, unsigned long* upper);
int GetGrubAPM(GrubInfo* info);

#endif
/* end of GRUB_H */
