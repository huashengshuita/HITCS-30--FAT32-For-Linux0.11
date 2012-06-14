#ifndef BOOTMEM_H
#define BOOTMEM_H

#define NR_HOLES             128

#define NIL_HOLE             NULL

struct mem_hole
{
    unsigned long base;
	  unsigned long size;
	  struct mem_hole * next_hole;
	
};

void            BootMemInit(void);
unsigned long   BootMemMalloc(unsigned long size);
void            BootMemFree(unsigned long base,unsigned long size);
void            BootMemScan(unsigned long *hole_size,unsigned long *hole_num);
void            BootMemEnd(unsigned long *mem_end);

#endif /* BOOTMEM_H */
