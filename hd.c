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
PUBLIC void task_hd()//Ӳ��������ѭ��
{
	MESSAGE msg;

	init_hd();//��ʼ������

	while (1) {
		send_recv(RECEIVE, ANY, &msg);

		int src = msg.source;

		switch (msg.type) {
		case DEV_OPEN://����DEV_OPEN��Ϣ
			hd_identify(0);//��ȡ����ӡ����Ӳ�̲���
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
PRIVATE void init_hd()//��ʼ��
{
	/* Get the number of drives from the BIOS data area */
	u8 * pNrDrives = (u8*)(0x475);//�������ַ0X475��ȡӲ������
	printl("NrDrives:%d.\n", *pNrDrives);
	assert(*pNrDrives);

	put_irq_handler(AT_WINI_IRQ, hd_handler);//ָ��hd_handlerΪӲ���жϴ�����򣬴�Ӳ���ж�
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
PRIVATE void hd_identify(int drive)//��ȡӲ�̲�������
{
	struct hd_cmd cmd;
	cmd.device  = MAKE_DEVICE_REG(0, drive, 0);//MAKE_DEVICE_REG�ϳ�DEVICE�Ĵ�������include/hd.h�ж���
	cmd.command = ATA_IDENTIFY;
	hd_cmd_out(&cmd);//��Ӳ�̷�������
	interrupt_wait();//�ȴ��жϷ���
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);//��DATA�˿ڻ�ȡ���ݣ��õ�Ӳ�̲���

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
PRIVATE void print_identify_info(u16* hdinfo)//������ӡ
{
	int i, k;
	char s[64];

	struct iden_info_ascii {
		int idx;
		int len;
		char * desc;
	} iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII���к� */
		     {27, 40, "HD Model"} /* Model number in ASCII �ͺ�*/ };

	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char * p = (char*)&hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2+1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		printl("%s: %s\n", iinfo[k].desc, s);
	}

	int capabilities = hdinfo[49];//���ܣ�֧��LBA���߼���Ѱַ����
	printl("LBA supported: %s\n",
	       (capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];//֧�ֵ����
	printl("LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];//�����������
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
PRIVATE void hd_cmd_out(struct hd_cmd* cmd)//��Ӳ����������������
{
	/**
	 * For all commands, the host must first check if BSY=1,
	 * and should proceed no further unless and until BSY=0
	 */
	if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT))//�ж�Status�Ĵ�����BSYλ
		panic("hd error.");

	/* Activate the Interrupt Enable (nIEN) bit */
	out_byte(REG_DEV_CTRL, 0);//ͨ��Device Control�Ĵ������ж�
	/* Load required parameters in the Command Block Registers ����Ҫ����д��Ĵ���*/
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR,  cmd->count);
	out_byte(REG_LBA_LOW,  cmd->lba_low);
	out_byte(REG_LBA_MID,  cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE,   cmd->device);
	/* Write the command code to the Command Register */
	out_byte(REG_CMD,     cmd->command);//һ��д��command�Ĵ��������ʼִ��
}

/*****************************************************************************
 *                                interrupt_wait
 *****************************************************************************/
/**
 * <Ring 1> Wait until a disk interrupt occurs.
 *
 *****************************************************************************/
PRIVATE void interrupt_wait()//�ȴ��ж�
{
	MESSAGE msg;
	send_recv(RECEIVE, INTERRUPT, &msg);//������Ϣ
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
 * <Ring 0�ں˲���> Interrupt handler.
 *
 * @param irq  IRQ nr of the disk interrupt.
 *****************************************************************************/
PUBLIC void hd_handler(int irq)//�жϴ������
{
	/*
	 * Interrupts are cleared when the host
	 *   - reads the Status Register,
	 *   - issues a reset, or
	 *   - writes to the Command Register.
	 */
	hd_status = in_byte(REG_STATUS);//��status�Ĵ����ָ��ж���Ӧ

	inform_int(TASK_HD);//֪ͨ��������
}
