//-----------------------------------------------------------------------------------------------------------------------
//          Phy Page Alloc For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "PageAlloc.h"
#include "Console.h"
#include "BootMem.h"
#include "Page.h"
#include "Process.h"
#include "String.h"
#include "Traps.h"
#include "CTYPE.h"

static  unsigned long HIGHT_MEM=0;                          //内存高端地址
static  unsigned long LOW_MEM=0;                            //内存低端地址

static  PageStruct* mem_map;

unsigned long  PAGING_PAGES=0;
                          
//------------------------------------------------------------------------------------------
//
//
//     物理内存分页表扩展（其中0~4MB 页表已初始化）
//
//
//
//-------------------------------------------------------------------------------------------
unsigned long PhysicalPageTableExpand(unsigned long end_mem)
{
	unsigned long  page_table_num=0;
	unsigned long  page_residue_num=0;    //不足一页表的剩余页面数
	unsigned long  index;
	unsigned long  page_table_addr;
	
  ConsolePrintf("Expand the physical page table ");

	page_table_num=(end_mem-0x400000)/(4096*1024);

	page_residue_num=((end_mem-0x400000)%(4096*1024))/4096;

	if(page_residue_num) page_table_num+=1;


	page_table_addr=(unsigned long)BootMemMalloc(page_table_num*4096);

	//重新设置Kernel Page_Dir
	//重新设置 VM 0~4MB 为空把 0 ~ 4MB 的映射断开
	for(index=0;index<1;index++)     
	{
	  //*(unsigned long *)(PG_DIR_ADDR+index*4) = 0;
	}


	//设置Kernel Page_Dir
	//设置物理空间 0 ~ 4MB 1 个页表
  for(index=0x300;index<0x301;index++)     
	{
	  *(unsigned long *)(__va(PG_DIR_ADDR)+index*4) = (PG_TABLE_ADDR+(index-0x300)*4096)|7;
	}

  //设置Kernel Page_Dir
	//设置物理空间 4MB ~ end_mem   page_table_num 个页表
  for(index=0x301;index<0x301+page_table_num;index++)     
	{
	  *(unsigned long *)(__va(PG_DIR_ADDR)+index*4) = (__pa(page_table_addr)+(index-0x301)*4096)|7;
	}

	//ConsoleMsg("  Expand Mem First 4MB page_table = %08x\n",page_table_addr);

	//设置扩展物理页表映射到物理内存 4MB ~ end_mem
	if(page_residue_num)
	for (index=0;index<0x400*(page_table_num-1)+page_residue_num;index++)	        
	{
    *((unsigned long*)(page_table_addr+index*4)) = (0x400000+index*4096)|7;
	}
	else
	for (index=0;index<0x400*page_table_num;index++)	        
	{
    *((unsigned long*)(page_table_addr+index*4)) = (0x400000+index*4096)|7;
	}

	InvalidatePageDir(PG_DIR_ADDR);

	ConsolePrintOK();

	if(page_residue_num)
	return (page_table_num-1)*1024+page_residue_num;
	else 
	return page_table_num*1024;

}


//------------------------------------------------------------------------------------------
//
//
//     内存页面扫描,并输出结果
//
//
//
//-------------------------------------------------------------------------------------------
int PageManagerScan(void)
{
    int i,free=0;

    for(i=0;i<PAGING_PAGES;i++)
    if(!TestPageUsed(&mem_map[i])) free++;

		//ConsoleMsg("  Total %d pages to alloc %d pages free\n",PAGING_PAGES,free); 
		return free;
}


//------------------------------------------------------------------------------------------
//
//
//     内存分页初始化
//
//
//
//-------------------------------------------------------------------------------------------
void PageManagerInit(unsigned long end_mem)
{
    unsigned long i;
		unsigned long page_num=0;
		unsigned long low_page_num=0;

    HIGHT_MEM=__va(end_mem);

    
		//在扩展了物理内存页面之后就不能在访问低端内存了 3GB 以下
    page_num=PhysicalPageTableExpand(end_mem);

		ConsoleMsg("  Expand %ld physical pages  total %ld KB\n",page_num,page_num*4);
		ConsoleMsg("  Physical page table set at vitrual address %08x\n",PAGE_OFFSET+PG_TABLE_ADDR);


		page_num=(end_mem)/4096;

    ConsolePrintf("Malloc the mem_map array for page alloc");
		mem_map=(PageStruct*)BootMemMalloc(page_num*sizeof(PageStruct));
		ConsolePrintOK();

		ConsoleMsg("  The mem_map array base is %08x size is %d KB \n",mem_map,page_num*sizeof(PageStruct)/1024);

		BootMemEnd(&LOW_MEM);

		ConsoleMsg("  The low mem of alloc page is %08x\n",LOW_MEM);


		low_page_num=(__pa(LOW_MEM)/4096)+1;   //向高端4KB 对齐

    PAGING_PAGES=page_num;

	
		for(i=0;i<low_page_num;i++)
		{
        SetPageReferenced(&mem_map[i]);                  //将内核页设为在内存中
				SetPageUsed(&mem_map[i]);                        //将内核页设为被占用
				SetPageLocked(&mem_map[i]);                      //将内核页设为锁定
				mem_map[i].count=0;
		}

    
		for(i=low_page_num;i<page_num;i++)
		{
        SetPageReferenced(&mem_map[i]);                 //将分配页设为在内存中
				SetPageUnUsed(&mem_map[i]);                     //将分配页设为被未占用
				SetPageUnLocked(&mem_map[i]);                   //将分配页设为被未锁定
				mem_map[i].count=0;
		}
		
		    
		PageManagerScan();
		
			

}


//------------------------------------------------------------------------------------------
//
//
//     内存页面映射表,找到一个空闲的页面,并将该页面的实际物理内存清零
//
//
//
//-------------------------------------------------------------------------------------------
unsigned long GetFreePage(void)
{
      unsigned long i;

      for(i=0;i<PAGING_PAGES;i++)
      if(!TestPageUsed(&mem_map[i]))                     //若找到了一个空闲的页
      {
         SetPageUsed(&mem_map[i]);

				 mem_map[i].count++;

         i<<=12;                                         //i*4K 得到实际的页偏移地址
				 i=__va(i);
                                                         //以下代码将页面的实际的物理地址清零    
				 /*
         __asm__("std; \n\t"
                 "movl %%ecx,%%edx\n\t"
                 "movl $1024,%%ecx\n\t"                  //ecx计数器置1024
                 "leal 4092(%%edx),%%edi\n\t"  
                 "rep: stosl\n\t"
                 "movl %%edx,%%eax"
                 :"=a"(__res)                            //输出的结果
                 :"c"(i));                               //将i送入ecx
				 */
			   memset((void*)i,0,4096);

				 return i;
      }

			KernelPanic("No enough physical memory to get a free page ");
      return 0;
}


//------------------------------------------------------------------------------------------
//
//
//     内存页面映射表,找到n个空闲的连续页面,并将该页面的实际物理内存清零
//
//
//
//-------------------------------------------------------------------------------------------
unsigned long GetFreePages(int page_num)
{
      unsigned long i;
			unsigned long start,end;
      int           num=0;

			start=end=0;

      for(i=0;i<PAGING_PAGES;i++)
      if(!TestPageUsed(&mem_map[i]))                     //若找到了一个空闲的页
      {
				 if(num==0) start=i;
				 num++;
				 if(num>=page_num) 
				 {
					 end=i;
					 break;
				 }
			}
			else if(num>0)
			{
				num=0;
				start=0;
			}

			for(i=start;i<=end;i++)
			SetPageUsed(&mem_map[i]);

      start<<=12;                                         // i*4K 得到实际的页偏移地址
			start=__va(start);
                                                         //以下代码将页面的实际的物理地址清零    
			memset((void*)start,0,page_num*4096);
		  return start;
}

//------------------------------------------------------------------------------------------
//
//
//     释放某一页面  addr 要释放的页面的物理地址
//
//
//
//-------------------------------------------------------------------------------------------
int FreePage(unsigned long addr)
{
     if(addr<LOW_MEM) 
     {
        return -1;
     } 

     if(addr>HIGHT_MEM)
     {
        return -1;
     }
     
     addr=__pa(addr);
     addr>>=12;                                    //把addr转换成页面号

     if(TestPageUsed(&mem_map[addr]) && !TestPageLocked(&mem_map[addr])) 
		 {
			 //ConsoleWarning("  Free addr %08x mem_mep.conut =%d\n",__va(addr<<12),mem_map[addr].count);
			 mem_map[addr].count--;

			 if(mem_map[addr].count<=0)
			 {
				  mem_map[addr].count=0;
				  SetPageUnUsed(&mem_map[addr]);
					return 0;
			 }
			 else return mem_map[addr].count;
		 }

		 return -2;
}


//------------------------------------------------------------------------------------------
//
//
//     添加某一页面  的使用计数
//
//
//
//-------------------------------------------------------------------------------------------
void AddPageCount(unsigned long addr)
{
	  if(addr<LOW_MEM) 
    {
        return;
    } 
    if(addr>HIGHT_MEM)
    {
        return;
    }

		addr=__pa(addr);
    addr>>=12;                                    //把addr转换成页面号
    if(TestPageUsed(&mem_map[addr]) && !TestPageLocked(&mem_map[addr])) 
		{
			mem_map[addr].count++;
		}

}

//------------------------------------------------------------------------------------------
//
//
//     在指定的物理地址处放置一页面,即将某一页的地址索引放入页表的指定位置处
//
//     page 要放置的页面真实物理地址,address 要放置页面指针的页表地址(线性地址)
//
//-------------------------------------------------------------------------------------------
unsigned long PutPage(unsigned long pg_dir,unsigned long page,unsigned long address)
{
        unsigned long tmp,*page_table;
				unsigned long page_phy_addr;
        
		
        if(page<LOW_MEM || page>HIGHT_MEM)        //若是要放入页表的页地址不在可分配空间中则出错
        {
           ConsoleWarning("put page error out of the page memory space 2MB~8MB addr %08x page %08x\n",
						               address,page);
           return 0;
        }

				page_phy_addr=__pa(page);

       
        if(!TestPageUsed(&mem_map[page_phy_addr>>12]))
        {
           ConsoleWarning("put page error mem map disgrees with the page num addr %08x page %08x\n",
						               address,page);
           return 0;
        }


        page_table=(unsigned long *)(pg_dir+((address>>20)&0xffc)); //找到address对应的实际在页目录中的地址

				//ConsolePrintf("  page_table %08x *page_table %08x\n",page_table,*page_table);

        if((*page_table)&1)
				{
            page_table=(unsigned long *)(0xfffff000 & *page_table);    //address对应的页表指针,注意为物理地址
						page_table=(unsigned long *)__va((unsigned long)page_table);
						AddPageCount((unsigned long)page_table);

				}
        else                                                 //若对应的页表指针不存在,则申请一个页面来存放页表
        {
           if(!(tmp=GetFreePage()))                          //<----注意这里在VM释放时要释放其页表所占页面
					 {ConsoleWarning("put page error --- you do not have enough memory!\n");return 0;}

           *page_table=__pa(tmp)|7;                           //注意要把页表的物理地址放入页目录
            page_table=(unsigned long *)tmp;                  //找到一个空闲页面来放页表
						//page_table=(unsigned long *)__va((unsigned long)page_table);
        }

				//ConsolePrintf("  page_table %08x page_table[%d]=%08x\n",page_table,(address>>12)&0x3ff,page_phy_addr|7);
        
        page_table[(address>>12)&0x3ff]=page_phy_addr | 7;

        return page;                
}

extern int nr_read;
//------------------------------------------------------------------------------------------
//
//
//     缺页故障处理函数
//
//     
//
//-------------------------------------------------------------------------------------------
unsigned long NoPage(unsigned long address)
{
	   unsigned long page;
		 unsigned long put_page=0;
		 int i;

     if(!(page=GetFreePage()))
		 return 0;

		 for(i=0;i<MAX_PROCESS;i++)
			 if(proc[i].procFlag==P_RUN)
		 {
		    if(!(put_page=PutPage(__va(proc[i].procTss.cr3),page,address))) return 0;
		    else if(i>0) AddPageCount((unsigned long)put_page);
		 }


		 if(ProcCurrent->pid!=0)
		 {
         InvalidatePageDir(ProcCurrent->procTss.cr3);
		 }
		 else InvalidatePageDir(PG_DIR_ADDR);

		 //ConsolePrintf(" Get a Free Page %08x\n",page);
		 //if(!(put_page=PutPage(__va(PG_DIR_ADDR),page,address))) return 0;
     //InvalidatePageDir(PG_DIR_ADDR);
		 //ConsolePrintf("Proc %d PGD %08x Put a new page %08x at address %08x\n",
		 //              ProcCurrent->pid,ProcCurrent->procTss.cr3,put_page,address);
		 return put_page;

}

//------------------------------------------------------------------------------------------
//
//
//     已对齐页面拷贝
//
//     内存管理中最重要的一部分!!!
//
//-------------------------------------------------------------------------------------------
int  AlignPageTableCopy(unsigned long from_pg_dir,unsigned long to_pg_dir,
									      unsigned long from,unsigned long to,unsigned long size)
{
   unsigned long j=0;

	 unsigned long* from_page_table=NULL;
	 unsigned long* to_page_table=NULL;

	 unsigned long new_page_table;

	 unsigned long page_num=size/4096;                //要拷的页面总数

	 unsigned long phy_addr;                          


	 if((size % 4096)) page_num++;

	 while(1)
	 {
     from_page_table=(unsigned long *)(from_pg_dir+((from>>20)&0xffc));   //找到from对应的实际在页目录中的地址
	   to_page_table=(unsigned long *)(to_pg_dir+((to>>20)&0xffc));         //找到to对应的实际在页目录中的地址


		 from_page_table=(unsigned long *)(0xfffff000 & *from_page_table);    //from对应的页表指针,注意为物理地址
		 from_page_table=(unsigned long *)__va((unsigned long)from_page_table);



		  if(!((*to_page_table)&1)) 
		  {
           if(!(new_page_table=GetFreePage()))                                 //新建目标页面表
	         {ConsoleWarning("copy page error --- you do not have enough memory!\n");return -2;}

		        *to_page_table=((unsigned long) __pa((unsigned long)new_page_table)) | 7;
		   }

		    to_page_table = (unsigned long *)(0xfffff000 & *to_page_table);         //获得要目的页面表的地址指针
		    to_page_table = (unsigned long*)__va((unsigned long)to_page_table);


	

		 for(j=0;j<1024;j++)
	   {
		    phy_addr=from_page_table[(from>>12)&0x3ff] & 0xfffff000;
				to_page_table[(to>>12)&0x3ff]=phy_addr | 7;

				//ConsoleMsg("Proc %d Copy Page %08x FromDir %08x ToDir %08x From %08x To %08x\n",
				//	          ProcCurrent->pid,from_pg_dir,to_pg_dir,phy_addr,from,to);
		    page_num--;
				if(page_num==0) return 1;
				from+=4096;
				to+=4096;

				if(((from>>12)&0x3ff)==0) break;                                   //超出了1023 则到下一页表去
				if(((to>>12)&0x3ff)==0) break;
	   }
	 }
}


