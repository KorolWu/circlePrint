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

//使用QnA 兼容3E 帧

//副标题 + 网络编号 + PLC编号 + I/O编号（请求目标模块）  + 站编号（请求目标模块） + 请求数据长度 + CPU监视定时器 + 命令 + 子命令 + 请求数据部分


//发送端
//Ascii ：5000 00 FF 03FF 00 0018 0010 （命令）*****
//H-L      4   2  2   4   2   4    4

//BIN(十六进制)：50 00   00   FF   FF 03  00   18 00    10 00  ****
//L-H			   2      1    1     2     1      2       2 （CPU监视定时器）

//反馈码
//BIN(十六进制)：D0 00   00   FF   FF 03  00   06 00   00 00   （应答数据）****
//L-H	           2      1    1     2     1     2       2（结束代码，非0标识有异常）



//            ASCII代码时    二进制代码时			 QnA兼容3E帧（软元件编号范围默认分配时）
//输入       X X*				9CH                  000000 - 0007FF
//输出       Y Y*				9DH                  000000 - 0007FF
//内部继电器 M M*			    90H                  000000 - 008191
//锁存继电器 L L*			    92H                  000000 - 008191
//步进继电器 S S*				98H                  000000 - 008191
//链接继电器 B B*				A0H                  000000 - 000FFF
//报警器     F F*				93H                  000000 - 002047
//特殊继电器 M M*				90H                  009000 - 009255

//数据寄存器 D D*               A8H                  000000 - 008191
//特殊寄存器 D D*               A8H                  009000 - 009255


#define CPUWAITTIME 0x0010

/**********软元件大小类型********/

#define BITCMD     0x0001
#define WORDCMD    0x0000

/**********位/字操作（单位由子命令决定，0x0000：按字为单位，0x0001：按位为单位）********/
//3.3.2
//按单位的成批读出
#define UnitBulkRead  0x0401
//按单位的成批写入
#define UnitBulkWrite 0x1401

/**********字（2字节）操作(子命令为0x0000)********/

//多块成批读出
#define WordBulkRead  0x0406
//多块成批写入
#define WordBulkWrite 0x1406


/**********随机读写（测试用,不建议使用）********/

//按单位的随机写(如果按字，分一个字和两个字)
#define UnitRamdWrite 0x1402
//字单位随机读出
#define WordRamdRead  0x0403


#pragma pack(push,1)

/******************** 协议区 *******************/

//请求与反馈时的数据头部分
//除了Title其他数据均是小端存储
typedef struct tagQnA3EHead
{
	//标签
	uint16 Title;
	//网络编号
	byte NetWorkNo;
	//PLC编号
	byte PLCNo;

	//I/O编号（请求目标模块）
	uint16 IONo;
	//站编号（请求目标模块）
	byte StationNo;
	//反馈数据长度(DataLen以下的数据总字节长度)
	uint16 DataLen;

}QnA3EHead;//9

//请求数据部分
typedef struct tagQnA3ECmd
{
	//命令头
	QnA3EHead CmdHead;
	//CPU监视定时器
	uint16 CPUTime;
	//命令
	uint16 MainCmd;
	//子命令
	uint16 ChildCmd;
	//输入的地址（高8bit为类型地址，低24bit为起始地址）
	uint32 AddrCmb;
	//点元个数
	uint16 STCount;

	//如果为写入指令，后面为数据内容

}QnA3ECmd;//21


//反馈数据部分
typedef struct tagQnA3EAck
{
	//反馈头
	QnA3EHead AckHead;
	//反馈结束码（非0标识有异常）
	uint16 EndCode;
	//如果为读取,则后面为读取的数据内容

}QnA3EAck;//11



/******************** 操作区 *******************/


//PLC存储区类型
enum PlcMemType
{
	//输入
	PMemDI = 1,
	//输出
	PMemDO = 2,
	//内部继电器
	PMemMR = 3,
	//数据寄存器(按字读取2字节)
	PMemDR = 4,
};

//表示方式
enum OpUnitType
{
	OPUTWord = 0,
	OPUTBit = 1,
};

struct PLCParam
{
	//起始地址
	uint32 StartAddr;
	//内存类型
	PlcMemType MemType;
	//PMemDR只能按字获取
	OpUnitType DataType;
	//点数
	uint16 Count;
};

#pragma pack(pop)
