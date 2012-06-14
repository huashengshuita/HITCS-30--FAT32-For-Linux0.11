//-----------------------------------------------------------------------------------------------------------------------
//          Page.C For FireXOS  1.01 
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "Page.h"
#include "GRUB.h"
#include "VBE.h"

//--------------------------------------------------------------------------------------------
//
// 启用分页寄存器
//
//---------------------------------------------------------------------------------------------
void PageEnable(void)
{
  unsigned long cr0;

  asm ("movl %%cr0, %0" : "=r" (cr0));
  asm ("movl %%eax, %%cr3": :"a"(PG_DIR_ADDR));

   /* set most significant bit of CR0 */
  cr0 |= 0x80000000;
  asm("movl %%eax, %%cr0": :"a"(cr0));
	asm ("jmp  1f\n\t"\
		   "1:\n\t"\
			 "movl $1f,%eax\n\t"\
       "jmp *%eax\n\t"\
			 "1:");

}

extern GrubInfo* GrubBootInfo;

//--------------------------------------------------------------------------------------------
//
// 页目录初始化
//
//---------------------------------------------------------------------------------------------
void	PageDirInit(void)
{
	unsigned long  index;
	unsigned long  VIDEOMEM;
	long           ret_ebp,ret_eip;
	long           save_ebp,save_esp;
  unsigned long  *LFBAddr = (unsigned long *) 0x80228;

	VIDEOMEM=LFBAddr[0];

  //初始化内核目录表
	//初始化内核空间的页表       64MB
	//设置0号页目录4GB空间为     U/S=0 R/W=0  P=0
	for(index=0;index<0x400;index++)
	{
    *(unsigned long*)(PG_DIR_ADDR+index*4) = 0;
	}

  //设置物理空间 0 ~ 4MB                     1 个页表
  for(index=0;index<0x001;index++)     
	{
	  *(unsigned long *)(PG_DIR_ADDR+index*4) = 
		 (PG_TABLE_ADDR+index*4096)|7;
	}


	//设置物理空间 3GB ~ 3GB + 4MB             1 个页表
  for(index=0x300;index<0x301;index++)     
	{
	  *(unsigned long*)(PG_DIR_ADDR+index*4) = 
		 (PG_TABLE_ADDR+(index-0x300)*4096)|7;
	}

	//设置剩下的 4GB-4MB ~ 4GB 空间为显存映射   1 个页表
	for(index=0x3FF;index<0x400;index++)
	{
    *(unsigned long*)(PG_DIR_ADDR+index*4) =
		 (VESA_PTE+(index-0x3FF)*4096)|7;
	}

	//设置0号页目录的1个页表映射到物理内存 0~4MB
	for (index=0;index<0x400;index++)	          //0x400 = 1k pages * 4KB= 4MB
	{
    *((unsigned long*)(PG_TABLE_ADDR+index*4)) = (index * 4096)|7;
	}


	//建立显存映射页表
	for (index=0;index<0x400;index++)         // 1k pages * 4KB =4 MB
	{
		*((unsigned long *)(VESA_PTE+index*4))=(VIDEOMEM+index*4096)|7;
	}


  SetCr3(GetCr3());
	PageEnable();
 
	
  //一下汇编代码改变该函数的返回地址使其指向3GB以上的内核虚地址
	//从而使内核从此在3GB以上的高端内存处运行

	__asm__("movl %%ebp,%0\n\t"
          "movl %%esp,%1\n\t"
          :"=m"(save_ebp),
           "=m"(save_esp));

  __asm__("movl %ebp,%esp");        

  __asm__("popl %0\n\t"
          :"=m"(ret_ebp));                 //获得返回的ebp
  __asm__("popl %0\n\t" 
          :"=m"(ret_eip));                 //获得返回的eip


	ret_eip+=0xC0000000;
	ret_ebp+=0xC0000000;

	__asm__("pushl %0\n\t"
		      : :"m"(ret_eip));                 //压入返回层的eip
	__asm__("pushl %0\n\t"
		      : :"m"(ret_ebp));                 //压入返回层的ebp

	__asm__("movl %0,%%ebp\n\t"
          "movl %1,%%esp\n\t"
          : :"m"(save_ebp+0xC0000000),
             "m"(save_esp+0xC0000000));


}

