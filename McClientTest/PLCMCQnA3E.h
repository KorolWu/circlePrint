#pragma once
#ifndef _PLCMCQA3E_H_
#define _PLCMCQA3E_H_
#endif

#ifndef  _BASICTYPE_H_

typedef  int int32;

typedef unsigned char byte;
typedef unsigned short uint16;
typedef unsigned int uint32;

#endif

//ʹ��QnA ����3E ֡

//������ + ������ + PLC��� + I/O��ţ�����Ŀ��ģ�飩  + վ��ţ�����Ŀ��ģ�飩 + �������ݳ��� + CPU���Ӷ�ʱ�� + ���� + ������ + �������ݲ���


//���Ͷ�
//Ascii ��5000 00 FF 03FF 00 0018 0010 �����*****
//H-L      4   2  2   4   2   4    4

//BIN(ʮ������)��50 00   00   FF   FF 03  00   18 00    10 00  ****
//L-H			   2      1    1     2     1      2       2 ��CPU���Ӷ�ʱ����

//������
//BIN(ʮ������)��D0 00   00   FF   FF 03  00   06 00   00 00   ��Ӧ�����ݣ�****
//L-H	           2      1    1     2     1     2       2���������룬��0��ʶ���쳣��



//            ASCII����ʱ    �����ƴ���ʱ			 QnA����3E֡����Ԫ����ŷ�ΧĬ�Ϸ���ʱ��
//����       X X*				9CH                  000000 - 0007FF
//���       Y Y*				9DH                  000000 - 0007FF
//�ڲ��̵��� M M*			    90H                  000000 - 008191
//����̵��� L L*			    92H                  000000 - 008191
//�����̵��� S S*				98H                  000000 - 008191
//���Ӽ̵��� B B*				A0H                  000000 - 000FFF
//������     F F*				93H                  000000 - 002047
//����̵��� M M*				90H                  009000 - 009255

//���ݼĴ��� D D*               A8H                  000000 - 008191
//����Ĵ��� D D*               A8H                  009000 - 009255


#define CPUWAITTIME 0x0010

/**********��Ԫ����С����********/

#define BITCMD     0x0001
#define WORDCMD    0x0000

/**********λ/�ֲ�������λ�������������0x0000������Ϊ��λ��0x0001����λΪ��λ��********/
//3.3.2
//����λ�ĳ�������
#define UnitBulkRead  0x0401
//����λ�ĳ���д��
#define UnitBulkWrite 0x1401

/**********�֣�2�ֽڣ�����(������Ϊ0x0000)********/

//����������
#define WordBulkRead  0x0406
//������д��
#define WordBulkWrite 0x1406


/**********�����д��������,������ʹ�ã�********/

//����λ�����д(������֣���һ���ֺ�������)
#define UnitRamdWrite 0x1402
//�ֵ�λ�������
#define WordRamdRead  0x0403


#pragma pack(push,1)

/******************** Э���� *******************/

//�����뷴��ʱ������ͷ����
//����Title�������ݾ���С�˴洢
typedef struct tagQnA3EHead
{
	//��ǩ
	uint16 Title;
	//������
	byte NetWorkNo;
	//PLC���
	byte PLCNo;

	//I/O��ţ�����Ŀ��ģ�飩
	uint16 IONo;
	//վ��ţ�����Ŀ��ģ�飩
	byte StationNo;
	//�������ݳ���(DataLen���µ��������ֽڳ���)
	uint16 DataLen;

}QnA3EHead;//9

//�������ݲ���
typedef struct tagQnA3ECmd
{
	//����ͷ
	QnA3EHead CmdHead;
	//CPU���Ӷ�ʱ��
	uint16 CPUTime;
	//����
	uint16 MainCmd;
	//������
	uint16 ChildCmd;
	//����ĵ�ַ����8bitΪ���͵�ַ����24bitΪ��ʼ��ַ��
	uint32 AddrCmb;
	//��Ԫ����
	uint16 STCount;

	//���Ϊд��ָ�����Ϊ��������

}QnA3ECmd;//21


//�������ݲ���
typedef struct tagQnA3EAck
{
	//����ͷ
	QnA3EHead AckHead;
	//���������루��0��ʶ���쳣��
	uint16 EndCode;
	//���Ϊ��ȡ,�����Ϊ��ȡ����������

}QnA3EAck;//11



/******************** ������ *******************/


//PLC�洢������
enum PlcMemType
{
	//����
	PMemDI = 1,
	//���
	PMemDO = 2,
	//�ڲ��̵���
	PMemMR = 3,
	//���ݼĴ���(���ֶ�ȡ2�ֽ�)
	PMemDR = 4,
};

//��ʾ��ʽ
enum OpUnitType
{
	OPUTWord = 0,
	OPUTBit = 1,
};

struct PLCParam
{
	//��ʼ��ַ
	uint32 StartAddr;
	//�ڴ�����
	PlcMemType MemType;
	//PMemDRֻ�ܰ��ֻ�ȡ
	OpUnitType DataType;
	//����
	uint16 Count;
};

#pragma pack(pop)
