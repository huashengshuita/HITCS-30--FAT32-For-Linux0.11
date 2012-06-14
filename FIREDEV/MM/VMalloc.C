//-----------------------------------------------------------------------------------------------------------------------
//          Virtual Mem Manager For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "BootMem.h"
#include "VMalloc.h"
#include "PageAlloc.h"
#include "CTYPE.h"
#include "Traps.h"
#include "Page.h"
#include "Process.h"
#include "Console.h"


static unsigned long VMEM_USER_BASE;
static unsigned long VMEM_SIZE;

static struct  mem_hole  hole[NR_HOLES];

static struct  mem_hole* hole_head;

static struct  mem_hole* free_slots;

static int     mem_nr=0;


//----------------------------------------------------------------------------------------------------------------------------
//
// 虚拟内存删除内存分配槽
//
//----------------------------------------------------------------------------------------------------------------------------

void VMemDelSlot(register struct mem_hole* prev_ptr,register struct mem_hole* hp)
{
	if(hp==hole_head)
	hole_head = hp->next_hole;
	else
	prev_ptr->next_hole=hp->next_hole;

	hp->next_hole = free_slots;
	free_slots=hp;
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 虚拟内存碎片合并
//
//----------------------------------------------------------------------------------------------------------------------------
void VMemMerge(register struct mem_hole* hp)
{

	register struct mem_hole* next_ptr;

	if((next_ptr = hp->next_hole)==NIL_HOLE) return;

	if(hp->base + hp->size == next_ptr->base)
	{
		hp->size+=next_ptr->size;

		VMemDelSlot(hp,next_ptr);
	}
	else
	{
		hp=next_ptr;
	}

	if(( next_ptr = hp->next_hole )== NIL_HOLE) return;

	if( hp->base + hp->size == next_ptr->base)
	{
		hp->size +=next_ptr->size;

		VMemDelSlot(hp,next_ptr);
	}
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 虚拟内存分配
//
//----------------------------------------------------------------------------------------------------------------------------

unsigned long VMalloc(unsigned long size)
{
  
	register struct mem_hole* hp, *prev_ptr=NULL;

	unsigned long old_base;

	if((size % 4096)!=0) size+=(4096-(size % 4096));  //向高端4KB 对齐

	hp = hole_head;

	while(hp != NIL_HOLE)
	{
		
		mem_nr++;

		if( hp->size >= size)
		{

			old_base = hp->base;
			hp->base+= size;
			hp->size-= size;

			if(hp->size !=0) return old_base;

			VMemDelSlot(prev_ptr,hp);
			return old_base;
		}

		prev_ptr=hp;
		hp=hp->next_hole;
	}

	return NULL;


}

//----------------------------------------------------------------------------------------------------------------------------
//
// 已对齐内存释放
//
//----------------------------------------------------------------------------------------------------------------------------
void VFreeAlignPageMap(unsigned long pg_dir,unsigned long base,unsigned long size)
{
   unsigned long  j=0;

	 unsigned long* page_table=NULL;

	 unsigned long  page_num=size/4096;

	 unsigned long  phy_addr;

	 if((size % 4096)) page_num++;

	 while(1)
	 {

	 page_table=(unsigned long *)(pg_dir+((base>>20)&0xffc)); //找到base对应的实际在页目录中的地址

	 //ConsoleWarning(" page_table = %08x *page_table = %08x\n",page_table,*page_table);

	 if((*page_table)&1)
	 {

       page_table=(unsigned long *)(0xfffff000 & *page_table);  //address对应的页表指针,注意为物理地址
			 page_table=(unsigned long *)__va((unsigned long)page_table);

			 for(j=0;j<1024;j++)
	     {
		       phy_addr=page_table[(base>>12)&0x3ff];
					 if(phy_addr & 1) 
					 {
						  phy_addr &= 0xfffff000;
		          if(FreePage(__va(phy_addr))==0)
					      page_table[(base>>12)&0x3ff]=0;
							if(FreePage((unsigned long)page_table)==0)                //释放页表页面
						    *((unsigned long *)(pg_dir+((base>>20)&0xffc)))=0;      //若此页表共享计数为零
		          page_num--;
					    //ConsolePrintf(" page num %d, page index %d\n",page_num,(base>>12)&0x3ff);
		          if(page_num==0) return;

					    base+=4096;
					    if(((base>>12)&0x3ff)==0) break;                           //超出了1023 则到下一页表去
					 }
					 else return;
	     }
			 //ConsolePrintf(" goto next table free page table %08x\n",page_table);
	 }
	 else return;


	 }
}


static char bFirstFree=1;
//----------------------------------------------------------------------------------------------------------------------------
//
// 虚拟内存释放
//
//----------------------------------------------------------------------------------------------------------------------------
void VFree(unsigned long base,unsigned long size)
{
  int i;

  register struct mem_hole* hp,*new_ptr, *prev_ptr;

	if(size==0) return;

	if((size % 4096)!=0) size+=(4096-(size % 4096));   //向高端4KB 对齐

	if((new_ptr=free_slots)==NIL_HOLE) KernelPanic("Hole Table Full !");

	if(!bFirstFree)
	{
		 for(i=0;i<MAX_PROCESS;i++)
		 if(proc[i].procFlag==P_RUN)
	   VFreeAlignPageMap(__va(proc[i].procTss.cr3),base,size);
		 //VFreeAlignPageMap(__va(PG_DIR_ADDR),base,size);

		 if(ProcCurrent->pid!=0)
     InvalidatePageDir(ProcCurrent->procTss.cr3);
	   else 
		 InvalidatePageDir(PG_DIR_ADDR);
	}

	bFirstFree=0;

	new_ptr->base=base;

	new_ptr->size=size;

	free_slots=new_ptr->next_hole;

	hp=hole_head;

	if( hp==NIL_HOLE || base<=hp->base)
	{
		new_ptr->next_hole=hp;

		hole_head=new_ptr;

		VMemMerge(new_ptr);

		return;
	}

	while( hp!=NIL_HOLE && base>hp->base)
	{
		prev_ptr=hp;

		hp=hp->next_hole;
	}

	new_ptr->next_hole = prev_ptr->next_hole;

	prev_ptr->next_hole = new_ptr;

	VMemMerge(prev_ptr);
}



//----------------------------------------------------------------------------------------------------------------------------
//
// 虚拟内存管理初始化
//
//----------------------------------------------------------------------------------------------------------------------------
void VMemInit(unsigned long vbase,unsigned long vsize)
{
	register struct mem_hole* hp;

	unsigned long base=VMEM_USER_BASE=vbase;

	unsigned long size=VMEM_SIZE=vsize;

	ConsolePrintf("Start the Virtual Memory Manager");

	bFirstFree=1;

	for(hp=&hole[0];hp<&hole[NR_HOLES];hp++)
	hp->next_hole=hp+1;

	hole[NR_HOLES-1].next_hole=NIL_HOLE;

	hole_head=NIL_HOLE;

	free_slots=&hole[0];

	VFree(base,size);

	ConsolePrintOK();
	ConsoleMsg("  Virtual Memory Alloc Start At %08x Virtual Memory Size is %d KB\n",vbase,vsize/1024);

}

//----------------------------------------------------------------------------------------------------------------------------
//
// 虚拟内存空洞链表扫描
//
//----------------------------------------------------------------------------------------------------------------------------
void VMemScan(unsigned long *hole_size,unsigned long *hole_num)
{
  register struct mem_hole* hp;

  hp=hole_head;

	*hole_size=0;

	*hole_num=0;

	while(hp!=NIL_HOLE)
	{

	    *hole_size += hp->size;

		  *hole_num = *hole_num +1;

		  hp=hp->next_hole;
	}

}








