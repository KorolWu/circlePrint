#ifndef MCCLIENT_H
#define MCCLIENT_H

#include "McClient_global.h"
#include "PLCMCQnA3E.h"
#include <QTcpSocket>
#include <QMutex>
#include <QHostAddress>
/**************** 自定义错误码 （小于等于0，大于0为系统级错误） *****************/

#define ERR_CTRLAPI_NOERR			    0  //0-正常

#define ERR_CTRLAPI_DISCON            -1  //通讯断开
#define ERR_CTRLAPI_TIMEOUT			  -2  //超时
#define ERR_CTRLAPI_FUNCNO            -3  //反馈与命令功能码不一致
#define ERR_CTRLAPI_INPUTERR          -4  //输入参数无效

#define ERR_CTRLAPI_ACKERR            -5  //反馈结果不正确

#define ERR_CTRLAPI_PLCHANDSHAKE      -100 //PLC握手失败
#define ERR_CTRLAPI_PLCWRITE          -101 //PLC写入失败
#define ERR_CTRLAPI_PLCINVALIDDATALEN -102 //PLC写入的参数长度与应该写入的长度不一致
#define ERR_CTRLAPI_PLCERRCODE        -103 //PLC异常码不为0

#define ERR_CTRLAPI_MEMALLOC           -200  //内存分配失败
#define ERR_CTRLAPI_MEMSMALL           -201
#define ERROR_SUCCESS                    0L

class MCCLIENT_EXPORT McClient: public QTcpSocket
{

private:
    QnA3EHead m_McCmdHead;
    QMutex *m_mutex;
    bool m_isOpen = false;
public:
    McClient();


protected:
    byte GetMenType(PlcMemType mem);
    //检查反馈的有效性
    int CheckAck(void * ackBuf, int dataLen);
    //设置命令内容
    //opType:0-读 1-写
    void SetCmdContent(const PLCParam & param, int opType, byte * buf, int bufLen);
    //发送字节，然后等待反馈
    int SendAndRecv(const byte* sndData, int dataBytes, byte * ack, int ackLen);
    /***************批量读写 适合在同一内存区的连续区域操作 ********************/
public:
    //批量写入
    //val：写入数据
    //num:写入的字节数
    int BulkWrite(const PLCParam & param, byte * val, int num, byte *wtBuf = nullptr, int bufSize = 0);
    //批量读取
    //buff:申请的内存由调用者通过delete[]释放
    //len:长度
    int BulkRead(const PLCParam & param, byte ** buff, int *len, byte *stBuf = nullptr, int bufSize = 0);
    //单个写入
    int WriteBit(PlcMemType type, uint32 stAddr, byte val);
    int WriteWord(PlcMemType type, uint32 stAddr, uint16 val);
    int WriteDouleWords(PlcMemType type, uint32 stAddr, uint32 val);

    //单个读取
    int ReadBit(PlcMemType type, uint32 stAddr, byte& val);
    int ReadWord(PlcMemType type, uint32 stAddr, uint16& val);
    int ReadDouleWords(PlcMemType type, uint32 stAddr, uint32& val);

    //批量读取
    //stCnt：访问的Word点元数量
    //val:申请的内存由调用者通过delete[]释放
    //dataOffset:有效数据的起始部分
    //num:val数据的长度
    int BulkReadWord(PlcMemType type, uint32 stAddr, int stCnt, byte ** val, int *dataOffset, int *num);

//socket 连接操作
    int Connect(QString ipAddress,uint16 port);

};

#endif // MCCLIENT_H
