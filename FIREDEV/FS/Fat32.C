//-----------------------------------------------------------------------------------------------------------------------
//          FAT32 FileSystem For FireXOS  1.01 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.10.2
//------------------------------------------------------------------------------------------------------------------------
#include "Fs.h"
#include "CTYPE.h"
#include "String.h"
#include "HardDisk.h"
#include "Msdos.h"
#include "Traps.h"
#include "PageAlloc.h"
#include "Console.h"


#define NR_FILE 32

static FILE MsOpenFile[NR_FILE];

static MSDOS_INFO Fat32_Info;

static unsigned char Fat32_RootDir[1024];

void PrintsFat32info(void);

void Fat32_LoadRootDir(void);
//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件系统初始化
//
//---------------------------------------------------------------------------------------------------------------------
int Fat32_Init(void)
{
	unsigned char buffer[512];
  MSDOS_SUPER* sp;

	ConsolePrintf("Init the fat32 filesystem");

	Fat32_Info.fs_base = hd[1].lowsec;

	memset(buffer,0,512);

	HardDiskRW(READ,Fat32_Info.fs_base,buffer);

  sp = (MSDOS_SUPER*)buffer;

  Fat32_Info.dos_sp.fats = sp->fats;
	Fat32_Info.cluster_size = sp->cluster_size;       //cluster size
  Fat32_Info.blk_size = sp->cluster_size*512;       //every block size.

	Fat32_Info.dos_sp.hidden = sp->hidden;           //unused
  Fat32_Info.dos_sp.reserved =  sp->reserved;

	Fat32_Info.dos_sp.fat16_length = sp->fat16_length;  //fat size
	Fat32_Info.dos_sp.fat32_length = sp->fat32_length;  //fat size
  strcpy(Fat32_Info.dos_sp.oem, sp->oem);

	Fat32_Info.dos_sp.total_sectors = sp->total_sectors;
	Fat32_Info.sector_size = sp->sector_size[0] + (sp->sector_size[1]<<8);
	Fat32_Info.dir_entries = sp->dir_entries[0] + (sp->dir_entries[1]<<8);

	Fat32_Info.fat_base =  Fat32_Info.fs_base + Fat32_Info.dos_sp.reserved;   
  
     if (sp->fat16_length != 0)
	{

	 Fat32_Info.flag = FAT16;
	 Fat32_Info.fat_size = sp->fat16_length;
	 Fat32_Info.fat_entries = sp->fat16_length*512/(sizeof(unsigned short));

	  //if(Fat32_Info.fat_entries <= FAT12_MAGIC)
    //msdos_loadfat12hd(dev);

	 Fat32_Info.fat_root =  Fat32_Info.fat_base + (sp->fats*sp->fat16_length);  //Value is sector
   Fat32_Info.blk_base =  Fat32_Info.fat_root + 32;  //32 origin: (dos_info.dir_entries*32)/512;
	 //dos_root= msdos_iget(dev, FAT12_ROOT);
	 }
	 else{

		 Fat32_Info.flag = FAT32;
		 Fat32_Info.fat_size = sp->fat32_length;
		 Fat32_Info.fat_entries = ((int)sp->fat32_length*512) / sizeof(unsigned long);

	   Fat32_Info.fat_root = sp->fat32_root_cluster;  //But this value is cluster.
     Fat32_Info.blk_base = Fat32_Info.fat_base + ((int)sp->fats * (int)sp->fat32_length);
		 //dos_root = msdos_iget(dev, dos_info.fat_root);
		}

		  //super[dev].root = dos_root;
      //super[dev].ops = &msdos_ops;	

  memset(MsOpenFile,0,32*sizeof(FILE));

  ConsolePrintOK();


	if(Fat32_Info.flag==FAT32)
	PrintsFat32info();

	Fat32_LoadRootDir();
	//Fat32_PrintRootDir();

	return 1;
}

//---------------------------------------------------------------------------------------------------------------------
//
//  显示 Fat32 文件系统信息
//
//---------------------------------------------------------------------------------------------------------------------
void PrintsFat32info(void)
{
	ConsoleMsg("  fat32 oem is : %s\n",Fat32_Info.dos_sp.oem);
	ConsoleMsg("  fat32 cluster size is : %ld\n",Fat32_Info.cluster_size);
	ConsoleMsg("  fat32 fat filenum is : %ld\n",Fat32_Info.dos_sp.fats);
	ConsoleMsg("  fat32 length   is : %ld\n",Fat32_Info.dos_sp.fat32_length);
	ConsoleMsg("  fat32 blk  base  is : %ld\n",Fat32_Info.blk_base);
	ConsoleMsg("  fat32 root enter is : %ld\n",Fat32_Info.fat_root);
	ConsoleMsg("  fat32 fat  enter is : %ld\n",Fat32_Info.fat_base);
}


//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 簇号转扇区号
//
//---------------------------------------------------------------------------------------------------------------------
int BlkToSec( int cluster )
{
	if(cluster > Fat32_Info.fat_entries)
	   KernelPanic("A Bad Fat32 Cluster!\n");

    if ( cluster > 1 )
	    return Fat32_Info.blk_base+(cluster-2) * Fat32_Info.cluster_size;
    
	return Fat32_Info.fat_root;  //root sector
}


//---------------------------------------------------------------------------------------------------------------------
//
//  从MS FAT32 表中查找某一簇的下一簇
//
//---------------------------------------------------------------------------------------------------------------------
unsigned long Fat32Next(unsigned int cluster)
{
	int blk, offset;
	unsigned long	get=0;
	unsigned long fat[128];

	int fats_per_sec = (512/sizeof(unsigned long));

  cluster &= 0xfffffff;
	if(cluster > Fat32_Info.fat_entries)
	   KernelPanic("A Bad Fat32 Cluster!\n");

	blk = (Fat32_Info.fat_base+(cluster/fats_per_sec));
	offset = (cluster % fats_per_sec);

	HardDiskRW(READ, blk, fat);
  get = fat[offset];
  return get;
}

//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件读取 （从指定的簇号和簇内偏移开始读取指定大小的数据)
//
//---------------------------------------------------------------------------------------------------------------------
int Fat32Reads(int cluster, void *buff, int offset, int size)
{
  int i, sector = 0; 
  int sec_num;
  unsigned char residual[512];

  if(size == 0)return 0;
  if(cluster > Fat32_Info.fat_entries)return -1;

     size = (size > (Fat32_Info.blk_size)) ? Fat32_Info.blk_size : size;

     sec_num = (size+512-1)/512;                            //will read sector number.
     sector =  BlkToSec(cluster);
	   sector += (offset%Fat32_Info.cluster_size);

     if(sec_num == 1){
	   HardDiskRW(READ, sector, residual);
	   memcpy(buff, residual, size);
      }
    
	   else if(sec_num>1&&(size%512) == 0)
	   for (i=0; i<sec_num; i++)
	   {
	     HardDiskRW(READ, sector+i, buff);
	     buff += 512;
	   }
	  else{
    for(i = 0;i < sec_num-1;i++)
    {
      HardDiskRW(READ, sector + i, buff);
	    buff += 512;
	  }

	  HardDiskRW(READ, sector+sec_num, residual);
	  memcpy(buff, residual, (size%512));
    }

		return 1;
}


//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件系统初始化
//
//---------------------------------------------------------------------------------------------------------------------
int Fat32ReadData(int first_cluster, unsigned char* data)
{
	//This Function Used by Read a Existed File all  data to buffer.
	int next, i = 0;
    
	if(first_cluster <0)return 1;
	next = first_cluster;
	while (1)
	{
		Fat32Reads(next,data+(Fat32_Info.blk_size*i), 0, Fat32_Info.blk_size);
    i++;

		next = Fat32Next(next);

		if (next == 0 || EOF_FAT32 || EOF_FAT16 || EOF_FAT12)
		break;
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
//
//  读取软盘根目录
//
//---------------------------------------------------------------------------------------------------------------------
void Fat32_LoadRootDir(void)
{
	int i;
	unsigned char* p=NULL;
	//Fat32_RootDir=(MSDOS_DIR*)VMalloc(1024);
	p=(unsigned char*)Fat32_RootDir;
	for(i=0;i<2;i++)
	{
		HardDiskRW(READ,BlkToSec(Fat32_Info.fat_root),p);
		p+=512;
	}
}

//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件名比较
//
//---------------------------------------------------------------------------------------------------------------------
int Fat32_Strcmp(const char* des_filename,const char* src_filename)
{
	int i=0;
	while(i<8) {
		if(des_filename[i]!=src_filename[i]) return 0;
		i++;
	}
	return 1;
}


void Fat32_PrintRootDir(void)
{
	MSDOS_DIR* p=NULL;
	p=(MSDOS_DIR*) Fat32_RootDir;
  int i=0;

	while(p->file_name[0]!=0x00)
	{
		ConsoleMsg("  Fat32 Root Dir File[%d] : %s\n",i++,(char*)p->file_name);
		p++;
	}
}

int nr_read=0;
//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件读取
//
//---------------------------------------------------------------------------------------------------------------------
int Fat32_Read(FILE* fp, void* buff, int __sb)
{

 	  int copied = 0;

	  unsigned int offset, read_size;
	  unsigned char temp[512*16];
	  int size;
		unsigned long curblk=0;

	  if(fp == NULL_FCB)
		KernelPanic("file is NULL");
   
	  if(fp->f_size <= fp->current_pos)
		return 0;

	  if((size=__sb) == 0){
        ConsoleWarning("fp_read(): file size: %d But will read is -%d\n", fp->f_size, size);
		return 0;
	  }

		//ConsoleMsg("  Start To Read the file fp->size %d fp->curblk %d\n",fp->f_size,fp->current_blk);

	  offset = fp->current_pos % Fat32_Info.blk_size;

	  while (size>0)
	  {
      //must be first
	    if(fp->f_size <= fp->current_pos)
		  break;

		  if( (fp->current_pos + size) > fp->f_size)
			  size = fp->f_size - fp->current_pos;

		  read_size = (size>=Fat32_Info.blk_size) ? Fat32_Info.blk_size : size;

	    if(offset == 0)
	    {  
       Fat32Reads(fp->current_blk, temp, 0, read_size);
	     memcpy(buff, (void *)temp, read_size);
	    }  	   
	   else{  
	   //if file offset not zero
	      if((fp->current_pos + read_size) > fp->f_size )
	       read_size = (fp->f_size - fp->current_pos);

        if ((fp->current_pos + offset) > Fat32_Info.blk_size)
			   read_size = (fp->current_pos + offset) - Fat32_Info.blk_size;

	      Fat32Reads(fp->current_blk, temp, 0, read_size);
	      memcpy(buff, (void *)(temp+offset), read_size);
	      offset = 0;  //reset offset come on.
        }

       //common object


	      (char *)buff += read_size;
        copied += read_size;
	      fp->current_pos += read_size;
	      size -= read_size;

				curblk=fp->current_blk;

	      if(!(fp->current_pos%Fat32_Info.blk_size))
	       fp->current_blk = Fat32Next(fp->current_blk);  //read next cluster

	      if(fp->current_blk >= EOF_FAT12 || fp->f_size <= fp->current_pos)
		    break;
	  }
	
		//ConsoleWarning("Read File Done!\n");
		//nr_read++;
    return copied;

}



//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件读取
//
//---------------------------------------------------------------------------------------------------------------------
FILE* Fat32_GetFCB(void)
{
  FILE* temp;

	for (temp = &MsOpenFile[0]; temp<&MsOpenFile[NR_FILE]; temp++)
	{
		if (temp->f_mode == UNUSED)
		return temp;
	}

	return NULL;
}

//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件读取
//
//---------------------------------------------------------------------------------------------------------------------
FILE* Fat32_OpenFile(char* filename)
{
	FILE* fp;

	MSDOS_DIR* p=NULL;
	fp=Fat32_GetFCB();

	p=(MSDOS_DIR*)Fat32_RootDir;

	while(p->file_name[0]!=0x00)
	{
		
		if(Fat32_Strcmp(filename,(char*)p->file_name))		
		//if(p->file_name[0]==filename[0])
		{  
			fp->current_blk=p->first_cluster;
			fp->current_pos=0;
			fp->f_size=p->file_size;
			return fp;
		}
		p++;
		
	}

	return NULL;

}

//FILE* Fat32Open(MSDOS_DIR* pDir,char* filename)
//{

//}
