
//-----------------------------------------------------------------------------------------------------------------------
//          Floppy Driver Header For FireXOS  1.01 
//
//          Version  1.01
//
//          Create by  xiaofan                                   2004.9.20
//
//          Update by xiaofan                                   2004.9.20
//------------------------------------------------------------------------------------------------------------------------

#ifndef FLOPPY_H
#define FLOPPY_H

#define NR_BLK_REQUEST	32

typedef struct blk_dev_request {
	int flag;		/* -1 if no request */
	int sector;
	int cmd;
	unsigned char *buffer;
	struct blk_dev_request *next;
	//struct semaphore sem;
}blk_req;


#define FD_STATUS 0x3f4                                      // ��״̬�Ĵ����˿ڡ�
#define FD_DATA   0x3f5                                      // ���ݶ˿ڡ�
#define FD_DOR    0x3f2                                      // ��������Ĵ�����Ҳ��Ϊ���ֿ��ƼĴ�������
#define FD_DIR    0x3f7                                      // ��������Ĵ�����
#define FD_DCR    0x3f7                                      // ���ݴ����ʿ��ƼĴ�����

/* ��״̬�Ĵ���������λ�ĺ��� */

#define STATUS_BUSYMASK 0x0F                                 // ������æλ��ÿλ��Ӧһ������������
#define STATUS_BUSY 0x10                                     // ���̿�����æ��
#define STATUS_DMA 0x20                                      // 0 - ΪDMA ���ݴ���ģʽ��1 - Ϊ��DMA ģʽ��
#define STATUS_DIR 0x40                                      // ���䷽��0 - CPU .. fdc��1 - �෴��
#define STATUS_READY 0x80                                    // ���ݼĴ�������λ��


/*״̬�ֽ�0��ST0��������λ�ĺ��� */

#define ST0_DS 0x03                                           // ������ѡ��ţ������ж�ʱ�������ţ���
#define ST0_HA 0x04                                           // ��ͷ�š�
#define ST0_NR 0x08                                           // ����������δ׼���á�
#define ST0_ECE 0x10                                          // �豸��������ŵ�У׼������
#define ST0_SE 0x20                                           // Ѱ��������У������ִ�н�����
#define ST0_INTR 0xC0                                         // �жϴ���λ���ж�ԭ�򣩣�00 - ��������������
                                                                                   // 01 - �����쳣������10 - ������Ч��11 - FDD ����״̬�ı䡣

/*״̬�ֽ�1��ST1��������λ�ĺ��� */

#define ST1_MAM 0x01                                           // δ�ҵ���ַ��־(ID AM)��
#define ST1_WP 0x02                                            // д������
#define ST1_ND 0x04                                            // δ�ҵ�ָ����������
#define ST1_OR 0x10                                            // ���ݴ��䳬ʱ��DMA ���������ϣ���
#define ST1_CRC 0x20                                           // CRC �������
#define ST1_EOC 0x80                                           // ���ʳ���һ���ŵ��ϵ���������š�

/*״̬�ֽ�2��ST2��������λ�ĺ��� */

#define ST2_MAM 0x01                                          // δ�ҵ����ݵ�ַ��־��
#define ST2_BC 0x02                                           // �ŵ�����
#define ST2_SNS 0x04                                          // ������ɨ�裩���������㡣
#define ST2_SEH 0x08                                          // �����������㡣
#define ST2_WC 0x10                                           // �ŵ������棩�Ų�����
#define ST2_CRC 0x20                                          // ���ݳ�CRC У���
#define ST2_CM 0x40                                           // ����������ɾ����־��

/*״̬�ֽ�3��ST3��������λ�ĺ��� */

#define ST3_HA 0x04                                            // ��ͷ�š�
#define ST3_TZ 0x10                                            // ��ŵ��źš�
#define ST3_WP 0x40                                            // д������


/* ���������� */

#define FD_RECALIBRATE 0x07                                    // ����У��(��ͷ�˵���ŵ�)��
#define FD_SEEK 0x0F                                           // ��ͷѰ����
#define FD_READ 0xE6                                           // �����ݣ�MT ��ŵ�������MFM ��ʽ������ɾ�����ݣ���
#define FD_WRITE 0xC5                                          // д���ݣ�MT��MFM����
#define FD_SENSEI 0x08                                         // ����ж�״̬��
#define FD_SPECIFY 0x03                                        // �趨�������������������ʡ���ͷж��ʱ��ȣ���


/* DMA ���� */
#define DMA_READ 0x46                                  // DMA ���̣�DMA ��ʽ�֣���DMA �˿�12��11����
#define DMA_WRITE 0x4A

//---------------------------------------------------------------------------------------------------------------------------------------------
// ���������ӿں���
//---------------------------------------------------------------------------------------------------------------------------------------------

void FloppyInit(void);
void FloppyMotorOn( void );
void FloppyMotorOff( void );
void FloppyDelay(void);
void FloppyRead(int blk,int secs,void* buf);


#endif

