/*************************************************************************//**
 *****************************************************************************
 * @file   main.c
 * @brief
 * 
 * 
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
//#include "config.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

#include "hd.h"



/*****************************************************************************
 *                                task_fs
 *****************************************************************************/
/**
 * <Ring 1> The main loop of TASK FS.文件系统进程
 * 其实是个空壳
 *****************************************************************************/
PUBLIC void task_fs()
{
	printl("Task FS begins.\n");

	/* open the device: hard disk */
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;//向硬盘驱动发送一个DEV_OPEN消息
	send_recv(BOTH, TASK_HD, &driver_msg);

	spin("FS");
}
