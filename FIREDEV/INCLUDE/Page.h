#ifndef PAGE_H
#define PAGE_H

#define PG_DIR_ADDR             0x100000L      //页目录地址
#define PG_TABLE_ADDR           0x101000L      //页表地址
#define PG_DIR_SIZE             0x1000L        //页目录大小   4KB
#define VESA_PTE                PG_TABLE_ADDR+0x1000
//#define PG_DIR_BASE(n)          (PG_DIR_ADDR+(n)*PG_DIR_SIZE)

#define PAGE_OFFSET             0xC0000000


# define __pa(x)		((x) - PAGE_OFFSET)
# define __va(x)		((x) + PAGE_OFFSET)             



#define SetCr3(x) {\
	      __asm__ __volatile__ (\
        "movl %%eax,%%cr3\n"\
		    : :"a" ( x ));}

//刷新页表高速缓冲区,只要将页表的基址再次送入cr3寄存器,电脑就会自动刷新
#define InvalidatePageDir(pg_dir) \
        __asm__("movl %%eax,%%cr3": :"a"(pg_dir))

extern __inline unsigned long GetCr3 (void)
{
  register unsigned long x;
  __asm__ __volatile__ ("movl %%cr3,%0" : "=r" (x));
  return (x);
}

typedef struct SysDesc
{
  unsigned short  limitLow;       //3fff, 64M
  unsigned short  baseLow;        //in kernel cs is 0
  unsigned char   baseMiddle;
  unsigned char   access;         //Attribute Bit: 9a (k: cs)
  unsigned char   granularity;    //c0
  unsigned char   baseHigh;
}SysDesc;

extern SysDesc gdt[256];

struct TssStruct {
  long backlink;  //连接字，保留前面任务的选择子
  long sp0;       //0级堆栈指针（特权最高）
  long ss0;       /* 高16位为零 */
  long sp1;       //1级堆栈指针
  long ss1;       /* 高16位为零 */
  long sp2;       //2级堆栈指针
  long ss2;       /* 高16位为零 */
  long cr3;       //CR3，用于地址映射
  long eip;       //以下是寄存器保存区域
  long eflags;
  long eax;
  long ecx;
  long edx;
  long ebx;
  long esp;
  long ebp;
  long esi;
  long edi;
  long es; /* 高16位为零 */
  long cs; /* 高16位为零 */
  long ss; /* 高16位为零 */
  long ds; /* 高16位为零 */
  long fs; /* 高16位为零 */
  long gs; /* 高16位为零 */
  long ldt;  /*由LDTR来确定  ，高16位为零 */
  unsigned short trap;  //TTS的特别属性字
  unsigned short iobase;  //指向IO许可位图的指针
  unsigned char  iomap[4]; //IO许可位图的结束字节
};

void	        PageDirInit(void);
unsigned long PutPage(unsigned long pg_dir,unsigned long page,unsigned long address);

#endif
/* end of Page.h */
