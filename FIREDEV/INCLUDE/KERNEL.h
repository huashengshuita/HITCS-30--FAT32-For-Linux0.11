#ifndef KERNEL_H
#define KERNEL_H

#include "CTYPE.h"

#define MEM 1
#define FD      2
#define HD     3
#define TTYX 4
#define TTY 5
#define LP 6
#define PIPE 7

#define KERNEL_NR_CHAR 64

/* Structure to hold boot parameters. */
struct bparam_s
{
  dev_t bp_rootdev;
  dev_t bp_bootdev;
  u32_t bp_ramsize;
  unsigned short bp_processor;
  unsigned char  bp_kernel[KERNEL_NR_CHAR];
  unsigned short bp_fs_type;
};


extern inline dev_t dev_nr(int ma, u8_t mi)
{
	ma &= 0007;
	mi &=  0xff;

	return ((unsigned)(ma << 8) + mi);
}

#endif 
/* end of KERNEL_H */

