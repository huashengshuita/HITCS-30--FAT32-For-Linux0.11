#ifndef CTYPE_H
#define CTYPE_H

#define NULL  0
#define FALSE 0
#define TRUE  1
#define SUCCESS 1

#define UNUSED 0


typedef unsigned char   u8_t;  //×Ö½Ú
typedef unsigned short u16_t;  //×Ö
typedef unsigned long  u32_t;  //Ë«×Ö
typedef unsigned long long int u64_t;

#define	U16_T(x)	(*(unsigned short *)(x))
#define	U32_T(x)	(*(unsigned long *)(x))

typedef	short	uid_t;
typedef	int	    pid_t;
typedef	unsigned long size_t;	/*  32 bits == 4 GB address space  */

typedef unsigned char gid_t;
typedef unsigned short dev_t;
typedef unsigned short ino_t;
typedef unsigned short mode_t;
typedef unsigned long time_t;
typedef long off_t;
typedef unsigned reg_t;
typedef unsigned short prot_t;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

typedef unsigned long HWND;
typedef unsigned int  HINSTANCE;
typedef unsigned long WPARAM;
typedef unsigned long LPARAM;
typedef unsigned char BOOL;


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#endif

