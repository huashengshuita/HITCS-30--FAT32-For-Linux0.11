//-----------------------------------------------------------------------------------------------------------------------
//          Boot Mem Manager For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "BootMem.h"
#include "CTYPE.h"
#include "Traps.h"
#include "Console.h"


#define BOOT_MEM_USER_BASE      0xC0103000
#define BOOT_MEM_SIZE           0x200000

static struct  mem_hole  hole[NR_HOLES];

static struct  mem_hole* hole_head;

static struct  mem_hole* free_slots;

static int     mem_nr=0;


//----------------------------------------------------------------------------------------------------------------------------
//
// 启动状态删除内存分配槽
//
//----------------------------------------------------------------------------------------------------------------------------

void BootMemDelSlot(register struct mem_hole* prev_ptr,register struct mem_hole* hp)
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
// 启动状态内存碎片合并
//
//----------------------------------------------------------------------------------------------------------------------------
void BootMemMerge(register struct mem_hole* hp)
{

	register struct mem_hole* next_ptr;

	if((next_ptr = hp->next_hole)==NIL_HOLE) return;

	if(hp->base + hp->size == next_ptr->base)
	{
		hp->size+=next_ptr->size;

		BootMemDelSlot(hp,next_ptr);
	}
	else
	{
		hp=next_ptr;
	}

	if(( next_ptr = hp->next_hole )== NIL_HOLE) return;

	if( hp->base + hp->size == next_ptr->base)
	{
		hp->size +=next_ptr->size;

		BootMemDelSlot(hp,next_ptr);
	}
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 启动状态内存分配
//
//----------------------------------------------------------------------------------------------------------------------------

unsigned long BootMemMalloc(unsigned long size)
{
  
	register struct mem_hole* hp, *prev_ptr=NULL;

	unsigned long old_base;

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

			BootMemDelSlot(prev_ptr,hp);
			return old_base;
		}

		prev_ptr=hp;
		hp=hp->next_hole;
	}

	return NULL;


}




//----------------------------------------------------------------------------------------------------------------------------
//
// 启动状态内存释放
//
//----------------------------------------------------------------------------------------------------------------------------
void BootMemFree(unsigned long base,unsigned long size)
{

  register struct mem_hole* hp,*new_ptr, *prev_ptr;
	if(size==0) return;

	if((new_ptr=free_slots)==NIL_HOLE) KernelPanic("Hole Table Full !");

	new_ptr->base=base;

	new_ptr->size=size;

	free_slots=new_ptr->next_hole;

	hp=hole_head;

	if( hp==NIL_HOLE || base<=hp->base)
	{
		new_ptr->next_hole=hp;

		hole_head=new_ptr;

		BootMemMerge(new_ptr);

		return;
	}

	while( hp!=NIL_HOLE && base>hp->base)
	{
		prev_ptr=hp;

		hp=hp->next_hole;
	}

	new_ptr->next_hole = prev_ptr->next_hole;

	prev_ptr->next_hole = new_ptr;

	BootMemMerge(prev_ptr);
}



//----------------------------------------------------------------------------------------------------------------------------
//
// 启动状态内存管理初始化
//
//----------------------------------------------------------------------------------------------------------------------------
void BootMemInit(void)
{
	register struct mem_hole* hp;

	long base=BOOT_MEM_USER_BASE;

	long size=BOOT_MEM_SIZE;

	ConsolePrintf("Start the Boot Memory Manager");

	for(hp=&hole[0];hp<&hole[NR_HOLES];hp++)
	hp->next_hole=hp+1;

	hole[NR_HOLES-1].next_hole=NIL_HOLE;

	hole_head=NIL_HOLE;

	free_slots=&hole[0];

	BootMemFree(base,size);

	ConsolePrintOK();

}

//----------------------------------------------------------------------------------------------------------------------------
//
// 内核主入口
//
//----------------------------------------------------------------------------------------------------------------------------
void BootMemScan(unsigned long *hole_size,unsigned long *hole_num)
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

//----------------------------------------------------------------------------------------------------------------------------
//
// 内核主入口
//
//----------------------------------------------------------------------------------------------------------------------------
void BootMemEnd(unsigned long *mem_end)
{
	register struct mem_hole* hp;


  hp=hole_head;

	*mem_end=BOOT_MEM_USER_BASE;

	while(hp!=NIL_HOLE)
	{

		if(hp->base>*mem_end)  {*mem_end = hp->base;}

		  hp=hp->next_hole;
	}

}







