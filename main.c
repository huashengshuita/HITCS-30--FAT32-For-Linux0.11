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
 * <Ring 1> The main loop of TASK FS.�ļ�ϵͳ����
 * ��ʵ�Ǹ��տ�
 *****************************************************************************/
PUBLIC void task_fs()
{
	printl("Task FS begins.\n");

	/* open the device: hard disk */
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;//��Ӳ����������һ��DEV_OPEN��Ϣ
	send_recv(BOTH, TASK_HD, &driver_msg);

	spin("FS");
}
