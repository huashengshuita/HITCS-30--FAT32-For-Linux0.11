/*************************************************************************//**
 *****************************************************************************
 * @file   hd.c
 * @brief  HD driver.
 * @author Forrest Y. Yu
 * @date   2005~2008
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
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


PRIVATE void	init_hd			();
PRIVATE void	hd_cmd_out		(struct hd_cmd* cmd);
PRIVATE int	waitfor			(int mask, int val, int timeout);
PRIVATE void	interrupt_wait		();
PRIVATE	void	hd_identify		(int drive);
PRIVATE void	print_identify_info	(u16* hdinfo);

PRIVATE	u8	hd_status;
PRIVATE	u8	hdbuf[SECTOR_SIZE * 2];

/*****************************************************************************
 *                                task_hd
 *****************************************************************************/
/**
 * Main loop of HD driver.
 *
 *****************************************************************************/
PUBLIC void task_hd()//硬盘驱动主循环
{
	MESSAGE msg;

	init_hd();//初始化工作

	while (1) {
		send_recv(RECEIVE, ANY, &msg);

		int src = msg.source;

		switch (msg.type) {
		case DEV_OPEN://接收DEV_OPEN消息
			hd_identify(0);//获取并打印部分硬盘参数
			break;

		default:
			dump_msg("HD driver::unknown msg", &msg);
			spin("FS::main_loop (invalid msg.type)");
			break;
		}

		send_recv(SEND, src, &msg);
	}
}

/*****************************************************************************
 *                                init_hd
 *****************************************************************************/
/**
 * <Ring 1> Check hard drive, set IRQ handler, enable IRQ and initialize data
 *          structures.
 *****************************************************************************/
PRIVATE void init_hd()//初始化
{
	/* Get the number of drives from the BIOS data area */
	u8 * pNrDrives = (u8*)(0x475);//从物理地址0X475获取硬盘数量
	printl("NrDrives:%d.\n", *pNrDrives);
	assert(*pNrDrives);

	put_irq_handler(AT_WINI_IRQ, hd_handler);//指定hd_handler为硬盘中断处理程序，打开硬盘中断
	enable_irq(CASCADE_IRQ);
	enable_irq(AT_WINI_IRQ);
}

/*****************************************************************************
 *                                hd_identify
 *****************************************************************************/
/**
 * <Ring 1> Get the disk information.
 *
 * @param drive  Drive Nr.
 *****************************************************************************/
PRIVATE void hd_identify(int drive)//获取硬盘参数函数
{
	struct hd_cmd cmd;
	cmd.device  = MAKE_DEVICE_REG(0, drive, 0);//MAKE_DEVICE_REG合成DEVICE寄存器，在include/hd.h中定义
	cmd.command = ATA_IDENTIFY;
	hd_cmd_out(&cmd);//向硬盘发送命令
	interrupt_wait();//等待中断发生
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);//从DATA端口获取数据，得到硬盘参数

	print_identify_info((u16*)hdbuf);
}

/*****************************************************************************
 *                            print_identify_info
 *****************************************************************************/
/**
 * <Ring 1> Print the hdinfo retrieved via ATA_IDENTIFY command.
 *
 * @param hdinfo  The buffer read from the disk i/o port.
 *****************************************************************************/
PRIVATE void print_identify_info(u16* hdinfo)//参数打印
{
	int i, k;
	char s[64];

	struct iden_info_ascii {
		int idx;
		int len;
		char * desc;
	} iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII序列号 */
		     {27, 40, "HD Model"} /* Model number in ASCII 型号*/ };

	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char * p = (char*)&hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2+1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		printl("%s: %s\n", iinfo[k].desc, s);
	}

	int capabilities = hdinfo[49];//功能：支持LBA（逻辑块寻址）否？
	printl("LBA supported: %s\n",
	       (capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];//支持的命令集
	printl("LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];//可用最大扇区
	printl("HD size: %dMB\n", sectors * 512 / 1000000);
}

/*****************************************************************************
 *                                hd_cmd_out
 *****************************************************************************/
/**
 * <Ring 1> Output a command to HD controller.
 *
 * @param cmd  The command struct ptr.
 *****************************************************************************/
PRIVATE void hd_cmd_out(struct hd_cmd* cmd)//向硬盘驱动器发送命令
{
	/**
	 * For all commands, the host must first check if BSY=1,
	 * and should proceed no further unless and until BSY=0
	 */
	if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT))//判断Status寄存器的BSY位
		panic("hd error.");

	/* Activate the Interrupt Enable (nIEN) bit */
	out_byte(REG_DEV_CTRL, 0);//通过Device Control寄存器打开中断
	/* Load required parameters in the Command Block Registers 将必要参数写入寄存器*/
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR,  cmd->count);
	out_byte(REG_LBA_LOW,  cmd->lba_low);
	out_byte(REG_LBA_MID,  cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE,   cmd->device);
	/* Write the command code to the Command Register */
	out_byte(REG_CMD,     cmd->command);//一旦写入command寄存器，命令开始执行
}

/*****************************************************************************
 *                                interrupt_wait
 *****************************************************************************/
/**
 * <Ring 1> Wait until a disk interrupt occurs.
 *
 *****************************************************************************/
PRIVATE void interrupt_wait()//等待中断
{
	MESSAGE msg;
	send_recv(RECEIVE, INTERRUPT, &msg);//接收消息
}

/*****************************************************************************
 *                                waitfor
 *****************************************************************************/
/**
 * <Ring 1> Wait for a certain status.
 *
 * @param mask    Status mask.
 * @param val     Required status.
 * @param timeout Timeout in milliseconds.
 *
 * @return One if sucess, zero if timeout.
 *****************************************************************************/
PRIVATE int waitfor(int mask, int val, int timeout)
{
	int t = get_ticks();

	while(((get_ticks() - t) * 1000 / HZ) < timeout)
		if ((in_byte(REG_STATUS) & mask) == val)
			return 1;

	return 0;
}

/*****************************************************************************
 *                                hd_handler
 *****************************************************************************/
/**
 * <Ring 0内核部分> Interrupt handler.
 *
 * @param irq  IRQ nr of the disk interrupt.
 *****************************************************************************/
PUBLIC void hd_handler(int irq)//中断处理程序
{
	/*
	 * Interrupts are cleared when the host
	 *   - reads the Status Register,
	 *   - issues a reset, or
	 *   - writes to the Command Register.
	 */
	hd_status = in_byte(REG_STATUS);//读status寄存器恢复中断响应

	inform_int(TASK_HD);//通知驱动程序
}
