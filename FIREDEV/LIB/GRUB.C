//-----------------------------------------------------------------------------------------------------------------------
//          GRUB Lib For FireXOS  1.10 
//
//          Version  1.10
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                    2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#include "GraphicDriver.h"
#include "Kernel.h"
#include "CType.h"
#include "GRUB.h"
#include "VBE.h"
#include "Console.h"

extern GrubInfo* GrubBootInfo;

//----------------------------------------------------------------------------------------------------------------------------
//
// 获取GRUB的版本信息
//
//----------------------------------------------------------------------------------------------------------------------------
int GetGrubVersion(GrubInfo* info)
{
	//struct VBEMode          *mode_info = (struct VBEMode*)       GrubBootInfo->vbe_mode_info;
  //struct VBEController    *control=    (struct VBEController*) GrubBootInfo->vbe_control_info;

    if (CHECK_GRUB(info->flags,9))
		{
				ConsoleMsg("  Boot Manager : %s\n",(char*)info->boot_loader_name);
		}
	return SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 获取GRUB的启动信息
//
//----------------------------------------------------------------------------------------------------------------------------
int GetGrubBoot(GrubInfo* info, int* DevNo)
{
	  int i, dev = -1;
 
    /*if microkernel here will be zero*/
    if (!CHECK_GRUB(info->flags,MULTIBOOT_BOOT_DEVICE)){
		for(i = 0; i<4; i++)
		ConsoleMsg("  Boot Device is %x\n", info->boot_device[i]);
		return -1;
    }

    /*if boot from floppy*/
    if (info->boot_device[3]<0x80){
	  dev = info->boot_device[3];  //if fp
	  ConsoleMsg("  Boot Kernel From FD%d\n",dev);
	  *DevNo = dev_nr(FD,0);
	  return dev_nr(FD,0);
	  }
    else {/*if boot from harddisk*/
		dev = info->boot_device[3]-0x80;
		// *dev_no = info->boot_device[2]+1;
    *DevNo = dev_nr(HD,dev*4+info->boot_device[2]+1);
	  ConsoleMsg("  Boot Kernel From hd%c (Linux Format)dev_no %d\n",'a'+dev, info->boot_device[2]+1);
	  return dev;
	}

	return dev;
}

//----------------------------------------------------------------------------------------------------------------------------
//
// 获取GRUB的可用内存信息
//
//----------------------------------------------------------------------------------------------------------------------------
int GetGrubMemInfo(GrubInfo* info, unsigned long* lower, unsigned long* upper)
{
   if (!CHECK_GRUB(info->flags, MULTIBOOT_MEMORY)) return -1;
		   ConsoleMsg("  Mem  lower = %dKB, Mem upper = %dKB\n",(int)info->mem_lower,(int)info->mem_upper);

   *upper = (unsigned long)((info->mem_upper+1024)*1024); 
   *lower = info->mem_lower;
   return SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------------
//
// 获取GRUB的高级电源管理列表
//
//----------------------------------------------------------------------------------------------------------------------------
int GetGrubAPM(GrubInfo* info)
{
    ConsoleMsg("  APM  table entry : " );

    if ( CHECK_GRUB( info -> flags, 10 ) ){
	  ConsoleMsg("found!\n");
	  return 0;
	}
     else
	  ConsoleMsg("not found!\n");
	  return -1;
}
