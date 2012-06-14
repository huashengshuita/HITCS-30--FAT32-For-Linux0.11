#ifndef VMALLOC_H
#define VMALLOC_H


void            VMemInit(unsigned long vbase,unsigned long vsize);
unsigned long   VMalloc(unsigned long size);
void            VFree(unsigned long base,unsigned long size);
void            VMemScan(unsigned long *hole_size,unsigned long *hole_num);

#endif /* BOOTMEM_H */

