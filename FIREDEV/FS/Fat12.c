//-----------------------------------------------------------------------------------------------------------------------
//          FAT12 FileSystem For FireXOS  1.10 
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.10.20
//------------------------------------------------------------------------------------------------------------------------

#include "CTYPE.h"
#include "Fs.h"
#include "Traps.h"
#include "Msdos.h"
#include "Floppy.h"
#include "String.h"
#include "Console.h"
#include "Timer.h"
#include "PageAlloc.h"

#define NR_FILE 32

static MSDOS_INFO    Fat12_Info;

FAT12_TAG            DosFat12[FAT12_MAGIC];

unsigned char*       Fat12_Temp;

static MSDOS_DIR*    Fat12_RootDir=NULL;

static FILE OpenFile[NR_FILE];



//---------------------------------------------------------------------------------------------------------------------
//
//  从MS FAT12表中查找某一簇的下一簇(有待研究）
//
//---------------------------------------------------------------------------------------------------------------------
unsigned short Msdos_12Next(const unsigned short cluster)
{
	unsigned short blk, next, offset;
	unsigned char  buf[512];

	if(cluster > Fat12_Info.fat_entries)
		KernelPanic("Next is a Bad Cluster!");

	blk = ((cluster*3)/2) / 512;
	offset = ((cluster*3)/2) % 512;


	FloppyRead(Fat12_Info.fat_base+blk, 1, buf);

	next=*(unsigned short *)(buf+offset);


	if((offset & 511) == 511){
		FloppyRead(Fat12_Info.fat_base+blk+1,1,buf);
		next=next+buf[0]*256;
	}


	if (cluster & 1) 
		next = next >> 4;   //1230 >> 0123
	else
		next &= 0x0fff;

	return next;
}


//---------------------------------------------------------------------------------------------------------------------
//
//  将字节串转为FAT12_TAG类型串
//
//---------------------------------------------------------------------------------------------------------------------
void Load_Fat12_Table(void)
{
	int i, nr;
	unsigned char* p=NULL;

	nr = Fat12_Info.fat_size;

	p = (unsigned char*)DosFat12;

	memset((void *)p, 0, FAT12_MAGIC);

	for (i=0; i<nr; i++)
	{    
		FloppyRead(Fat12_Info.fat_base+i, 1, p);
		p += 512;
	}


	if((*(unsigned char*)DosFat12 & 0xff) == 0xf0)
		ConsoleMsg("  Fat media is 1.44MB!\n");

	  ConsoleMsg("  FAT:[  %03x %03x %03x %03x %03x]\n",
		 DosFat12[0].fat_item, DosFat12[1].fat_item, DosFat12[2].fat_item,DosFat12[3].fat_item,DosFat12[4].fat_item);

}



//---------------------------------------------------------------------------------------------------------------------
//
//  簇号转扇区号
//
//---------------------------------------------------------------------------------------------------------------------
int Fat12_ToSec(int cluster)
{
	if(cluster > Fat12_Info.fat_entries)
		KernelPanic("Is a Bad Cluster!");

	if ( cluster > 1 )
		return Fat12_Info.blk_base+(cluster-2)*Fat12_Info.cluster_size;

	return Fat12_Info.fat_root;  //root sector
}

//---------------------------------------------------------------------------------------------------------------------
//
//  读取软盘根目录
//
//---------------------------------------------------------------------------------------------------------------------
void LoadRootDir(void)
{
	int i;
	unsigned char* p=NULL;
	Fat12_RootDir=(MSDOS_DIR*)GetFreePage();
	p=(unsigned char*)Fat12_RootDir;
	for(i=0;i<2;i++)
	{
		FloppyRead(Fat12_Info.fat_root+i,1,p);
		p+=512;
	}

}

//---------------------------------------------------------------------------------------------------------------------
//
//  Fat12 文件名比较
//
//---------------------------------------------------------------------------------------------------------------------
int Fat12_Strcmp(const char* des_filename,const char* src_filename)
{
	int i=0;
	while(i<8) {
		if(des_filename[i]!=src_filename[i]) return 0;
		i++;
	}
	return 1;
}


void PrintsFat12info(void);
//---------------------------------------------------------------------------------------------------------------------
//
//  Fat12 文件系统初始化
//
//---------------------------------------------------------------------------------------------------------------------
void Fat12_Init(void)
{
	unsigned char buffer[512];
	MSDOS_SUPER* sp;
  
	ConsolePrintf("Init the fat12 filesystem");

	memset(buffer,0,512);
	FloppyRead(0, 1, buffer);
	sp = (MSDOS_SUPER*)buffer;

	Fat12_Info.dos_sp.fats = sp->fats;
	Fat12_Info.cluster_size = sp->cluster_size;      //cluster size
	Fat12_Info.blk_size = sp->cluster_size*512;      //every block size.
	Fat12_Info.dos_sp.hidden = sp->hidden;           //unused
	Fat12_Info.dos_sp.reserved =  sp->reserved;

	Fat12_Info.dos_sp.fat16_length = sp->fat16_length;  //fat size fat 表所占扇区数
	strcpy((char*)Fat12_Info.dos_sp.oem, (char*)sp->oem);

	Fat12_Info.sector_size = sp->sector_size[0] + (sp->sector_size[1]<<8);
	Fat12_Info.dir_entries = sp->dir_entries[0] + (sp->dir_entries[1]<<8);

	Fat12_Info.fat_size = sp->fat16_length;
	Fat12_Info.flag = FAT12;
	Fat12_Info.fat_entries = sp->fat16_length*512*3/(sizeof(unsigned short));
	Fat12_Info.fat_root =  sp->fat16_length*sp->fats + sp->reserved + sp->hidden;
	Fat12_Info.fat_base =  sp->reserved;              //Value is sector
	Fat12_Info.blk_base =  Fat12_Info.fat_root + 14;  //14 origin: (fat12_info.dir_entries*32)/512;
	ConsolePrintOK();
	PrintsFat12info();
	//Load_Fat12_Table();
	LoadRootDir();
	TickDelay(1);
}


void PrintsFat12info(void)
{
	ConsoleMsg("\n");
	ConsoleMsg("  fat12 oem is : %s\n",Fat12_Info.dos_sp.oem);
	ConsoleMsg("  fat12 cluster size is : %d\n",Fat12_Info.cluster_size);
	ConsoleMsg("  fat12 fat filenum is : %d\n",Fat12_Info.dos_sp.fats);
	ConsoleMsg("  fat12 length   is : %d\n",Fat12_Info.dos_sp.fat16_length);
	ConsoleMsg("  fat12 blk  base  is : %d\n",Fat12_Info.blk_base);
	ConsoleMsg("  fat12 root enter is : %d\n",Fat12_Info.fat_root);
	ConsoleMsg("  fat12 fat  enter is : %d\n",Fat12_Info.fat_base);
	ConsoleMsg("\n");


}

//---------------------------------------------------------------------------------------------------------------------
//
//  读取 Fat12 文件
//
//---------------------------------------------------------------------------------------------------------------------
int Fat12_Read(FILE*fp, void *buff, unsigned long __sb)
{
	unsigned long copied = 0;
	unsigned long SECTOR, offset, read_size;
	unsigned char temp[512];
	unsigned long size;
	unsigned int  nr_blk=0;

	if(fp->current_pos >= fp->f_size){
		return 0;
	}

	if((size=__sb) == 0){
		return 0;
	}

	offset = fp->current_pos % 512;

	while (size>0)
	{
		SECTOR = Fat12_ToSec(fp->current_blk);

		if(fp->f_size <= fp->current_pos)  break;

		if( (fp->current_pos + size) > fp->f_size)
			size = fp->f_size - fp->current_pos;

		read_size = (size>=512) ? 512 : size;

		if(offset == 0)
		{ 
			FloppyRead(SECTOR, 1, (void *)temp);
			memcpy(buff, (void *)temp, read_size);
		}  	   
		else{  
			//if file offset not zero
			if((fp->current_pos + read_size) > fp->f_size )
				read_size = (fp->f_size - fp->current_pos);

			if ((fp->current_pos + offset) > 512)
				read_size = (fp->current_pos + offset) - 512;

			FloppyRead(SECTOR, 1, (void *)temp);
			memcpy(buff, (void *)(temp+offset), read_size);
			offset = 0;  //reset offset come on.
		}

		//common object
		(unsigned char *)buff += read_size;
		copied += read_size;
		fp->current_pos += read_size;
		size -= read_size;

		if(!(fp->current_pos%512))
			fp->current_blk = Msdos_12Next(fp->current_blk);  //read next cluster

		nr_blk++;
		if(fp->current_blk >= EOF_FAT12 || fp->f_size <= fp->current_pos)
			break;
	}

	ConsolePrintf("Read File %d Blk fp->current_pos %d blk %04x\n",nr_blk,fp->current_pos,fp->current_blk);

	return copied;
}


//---------------------------------------------------------------------------------------------------------------------
//
//  Fat32 文件读取
//
//---------------------------------------------------------------------------------------------------------------------
FILE* GetFCB(void)
{
  FILE* temp;

	for (temp = &OpenFile[0]; temp<&OpenFile[NR_FILE]; temp++)
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
FILE* Fat12_OpenFile(char* filename)
{
	FILE* fp;

	MSDOS_DIR* p=NULL;
	fp=GetFCB();
	p=Fat12_RootDir;

	while(p->file_name[0]!=0x00)
	{
		if(Fat12_Strcmp(filename,(char*)p->file_name))		
		{  
			//ConsolePrintf(" Find The File %s \n",p->file_name);
			fp->current_blk=p->first_cluster;
			fp->current_pos=0;
			fp->f_size=p->file_size;
			return fp;
		}
		p++;
		
	}

	return NULL;

}




