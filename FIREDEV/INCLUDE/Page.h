#ifndef PAGE_H
#define PAGE_H

#define PG_DIR_ADDR             0x100000L      //ҳĿ¼��ַ
#define PG_TABLE_ADDR           0x101000L      //ҳ���ַ
#define PG_DIR_SIZE             0x1000L        //ҳĿ¼��С   4KB
#define VESA_PTE                PG_TABLE_ADDR+0x1000
//#define PG_DIR_BASE(n)          (PG_DIR_ADDR+(n)*PG_DIR_SIZE)

#define PAGE_OFFSET             0xC0000000


# define __pa(x)		((x) - PAGE_OFFSET)
# define __va(x)		((x) + PAGE_OFFSET)             



#define SetCr3(x) {\
	      __asm__ __volatile__ (\
        "movl %%eax,%%cr3\n"\
		    : :"a" ( x ));}

//ˢ��ҳ����ٻ�����,ֻҪ��ҳ��Ļ�ַ�ٴ�����cr3�Ĵ���,���Ծͻ��Զ�ˢ��
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
  long backlink;  //�����֣�����ǰ�������ѡ����
  long sp0;       //0����ջָ�루��Ȩ��ߣ�
  long ss0;       /* ��16λΪ�� */
  long sp1;       //1����ջָ��
  long ss1;       /* ��16λΪ�� */
  long sp2;       //2����ջָ��
  long ss2;       /* ��16λΪ�� */
  long cr3;       //CR3�����ڵ�ַӳ��
  long eip;       //�����ǼĴ�����������
  long eflags;
  long eax;
  long ecx;
  long edx;
  long ebx;
  long esp;
  long ebp;
  long esi;
  long edi;
  long es; /* ��16λΪ�� */
  long cs; /* ��16λΪ�� */
  long ss; /* ��16λΪ�� */
  long ds; /* ��16λΪ�� */
  long fs; /* ��16λΪ�� */
  long gs; /* ��16λΪ�� */
  long ldt;  /*��LDTR��ȷ��  ����16λΪ�� */
  unsigned short trap;  //TTS���ر�������
  unsigned short iobase;  //ָ��IO���λͼ��ָ��
  unsigned char  iomap[4]; //IO���λͼ�Ľ����ֽ�
};

void	        PageDirInit(void);
unsigned long PutPage(unsigned long pg_dir,unsigned long page,unsigned long address);

#endif
/* end of Page.h */
