#ifndef PAGEALLOC_H
#define PAGEALLOC_H


#define PG_ERROR		     1            
#define PG_REFERENCED		 2             //在内存中
#define PG_UPTODATE		   4
#define PG_USED 		     8             //被使用中
#define PG_LOCKED		     16	           //锁定页,不允许换出




typedef struct{
   unsigned char FlagBitmap;
	 int           count; 
	 unsigned long index;

}PageStruct;

extern unsigned long  PAGING_PAGES;

extern __inline void SetPageFlagBitmap(PageStruct* page,unsigned char FlagBit)
{
	page->FlagBitmap |= FlagBit;
}

extern __inline void UnSetPageFlagBitmap(PageStruct* page,unsigned char FlagBit)
{
	page->FlagBitmap &= (~FlagBit);
}

extern __inline int TestPageFlagBitmap(PageStruct* page,unsigned char FlagBit)
{
	if(page->FlagBitmap & FlagBit) return 1;
	return 0;
}


#define SetPageLocked(page) SetPageFlagBitmap(page,PG_LOCKED)
#define SetPageError(page)  SetPageFlagBitmap(page,PG_ERROR)
#define SetPageReferenced(page) SetPageFlagBitmap(page,PG_REFERENCED)
#define SetPageUptoDate(page) SetPageFlagBitmap(page,PG_UPTODATE)
#define SetPageUsed(page) SetPageFlagBitmap(page,PG_USED)

#define SetPageUnLocked(page) UnSetPageFlagBitmap(page,PG_LOCKED)
#define SetPageUnError(page)  UnSetPageFlagBitmap(page,PG_ERROR)
#define SetPageUnReferenced(page) UnSetPageFlagBitmap(page,PG_REFERENCED)
#define SetPageUnUptoDate(page) UnSetPageFlagBitmap(page,PG_UPTODATE)
#define SetPageUnUsed(page) UnSetPageFlagBitmap(page,PG_USED)

#define TestPageLocked(page) TestPageFlagBitmap(page,PG_LOCKED)
#define TestPageError(page) TestPageFlagBitmap(page,PG_PG_ERROR)
#define TestPageReferenced(page) TestPageFlagBitmap(page,PG_REFERENCED)
#define TestPageUptoDate(page) TestPageFlagBitmap(page,PG_UPTODATE)
#define TestPageUsed(page) TestPageFlagBitmap(page,PG_USED)




unsigned long  PhysicalPageTableExpand(unsigned long end_mem);
void PageManagerInit(unsigned long end_mem);
int  FreePage(unsigned long addr);

//int CopyPageTables(unsigned long from_pg_dir,unsigned long to_pg_dir,
//									 unsigned long from,unsigned long to,unsigned long size);

int  AlignPageTableCopy(unsigned long from_pg_dir,unsigned long to_pg_dir,
									      unsigned long from,unsigned long to,unsigned long size);
int PageManagerScan(void);
unsigned long GetFreePage(void);
unsigned long GetFreePages(int page_num);
void SetPageTable(unsigned long pg_dir,unsigned long line_base,unsigned long phy_base,unsigned long page_num);
unsigned long NoPage(unsigned long address);

#endif
/* end of PageAlloc.h */
