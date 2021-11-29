#include "mcclient.h"

McClient::McClient()
{
    //标签
    m_McCmdHead.Title = 0x0050;
    //网络编号
    m_McCmdHead.NetWorkNo = 0x00;
    //PLC编号
    m_McCmdHead.PLCNo = 0xff;
    //I/O编号（请求目标模块）
    m_McCmdHead.IONo = 0x03ff;
    //站编号（请求目标模块）
    m_McCmdHead.StationNo = 0x00;
    //总长度
    m_McCmdHead.DataLen = 0x00;
    m_mutex = new QMutex();
}
//获取成批写入时的数据长度
inline int GetBulkWDataLen(const PLCParam & param)
{
    //0x0000：按字为单位，0x0001：按位为单位）
    if (param.DataType == OPUTBit)//位模式
        return (param.Count + 1) >> 1;//一个字节存放两个M值
    else//字模式
        return param.Count << 1;//若M区按字访问，则一个字节存放8个M值
}

//cpBytes:数据长度
inline int GetCmdLen(int cpBytes)
{
    //命令长度 + 数据长度（地址 + 点数 + 数据）
    return sizeof(QnA3ECmd) + cpBytes;
}

inline uint16 GetOpUnitType(OpUnitType type)
{
    switch (type)
    {
    case OPUTWord:
    default:
        return WORDCMD;
    case OPUTBit:
        return BITCMD;
    }
}

byte McClient::GetMenType(PlcMemType mem)
{
    switch (mem)
    {
    case PMemDI:
        return 0x9C;
    case PMemDO:
        return 0x9D;
    case PMemMR:
        return 0x90;
    case PMemDR:
        return 0xA8;
    default:
        // throw System::Except::Exception(_T("PlcMemory type out of range"));
        break;
    }
}

int McClient::CheckAck(void *ackBuf, int dataLen)
{
    QnA3EAck * ack;
    //获取反馈
    ack = (QnA3EAck *)ackBuf;

    if (ackBuf == nullptr)
        return ERR_CTRLAPI_INPUTERR;

    //数据头是否正确
    if (ack->AckHead.Title != 0x00d0)
        return ERR_CTRLAPI_ACKERR;

    //判断是否具长度
    if (ack->AckHead.DataLen != dataLen)
        return ERR_CTRLAPI_PLCINVALIDDATALEN;

    //判断返回值是否正确
    if (ack->EndCode != 0)
        return ERR_CTRLAPI_PLCERRCODE;

    return ERR_CTRLAPI_NOERR;
}

void McClient::SetCmdContent(const PLCParam &param, int opType, byte *buf, int bufLen)
{
    QnA3ECmd *cmd;

    cmd = (QnA3ECmd *)(buf);
    //1.拷贝数据头
    ::memcpy(buf, (void *)&m_McCmdHead, sizeof(m_McCmdHead) - sizeof(m_McCmdHead.DataLen));
    cmd->CmdHead.DataLen = (uint16)(bufLen - sizeof(QnA3EHead));//总长度不包含头的数据

    //2.拷贝命令(8字节)
    cmd->CPUTime = CPUWAITTIME;
    cmd->MainCmd = opType == 0 ? UnitBulkRead : UnitBulkWrite;
    cmd->ChildCmd = GetOpUnitType(param.DataType);

    //检查命令是否正确（D寄存器无法用位访问，将其转换成按字处理）
    if (param.MemType == PMemDR && param.DataType == OPUTBit)
        cmd->ChildCmd = WORDCMD;

    //3.拷贝数据
    //地址 + 点数 + 数据
    //填值计算
    cmd->AddrCmb = param.StartAddr | (GetMenType(param.MemType) << 24);
    cmd->STCount = param.Count;
}

int McClient::SendAndRecv(const byte *sndData, int dataBytes, byte *ack, int ackLen)
{
    if (!m_isOpen)
        return ERR_CTRLAPI_DISCON;
    if (dataBytes == 0 || ackLen == 0)
        return ERR_CTRLAPI_INPUTERR;
    int  errCode;
    qint64 ret;
    //已发送或接收的数据
    int sndLen = 0;
    m_mutex->lock();
    //再判断一次，防止端口关闭了而继续操作
    if (!m_isOpen)
    {
        errCode = ERR_CTRLAPI_DISCON;
        goto labErr;
    }
    //先清除缓冲区
    //ClearBuff();

    //写入
    while (true)
    {

        //ret = ::send(m_socket, (char*)(sndData + sndLen), dataBytes - sndLen, 0);
        ret = write((char*)(sndData+sndLen),dataBytes-sndLen);
        if (ret < 0)
        {
            errCode = ERR_CTRLAPI_INPUTERR;
            goto labErr;
        }
        else if (ret == -1)
        {
            errCode = ERR_CTRLAPI_DISCON;
            goto labErr;
        }
        else// if (res > 0)
        {
            sndLen += ret;
            //是否达到指定长度
            if (sndLen >= dataBytes)
                break;
        }
    }

    //接收量清零
    sndLen = 0;

    while (true)
    {
       // ret = ::recv(m_socket, (char*)(ack + sndLen), ackLen - sndLen, 0);
        bool readRead = waitForReadyRead();
        if( !readRead)
        {
             errCode = ERR_CTRLAPI_ACKERR;
             goto labErr;
        }
        ret = read( (char*)(ack + sndLen), ackLen - sndLen);
        if (ret < 0)
        {
            errCode = ERR_CTRLAPI_ACKERR;
            goto labErr;
        }
        else if (ret == -1)
        {
            errCode = ERR_CTRLAPI_DISCON;
            goto labErr;
        }
        else// if (res > 0)
        {
            sndLen += ret;
            //是否达到指定长度
            if (sndLen >= ackLen)
                break;
        }
    }

    m_mutex->unlock();

    return ERROR_SUCCESS;

labErr:
    m_mutex->unlock();
    return errCode;
}

int McClient::BulkWrite(const PLCParam &param, byte *val, int num, byte *wtBuf, int bufSize)
{
    byte *sndBuff, *ackBuf;
    int cpBytes, bufLen, ret, ackLen;

    //判断是否长度是否正确
    cpBytes = GetBulkWDataLen(param);
    if (cpBytes != num)
        return ERR_CTRLAPI_PLCINVALIDDATALEN;
    if (val == nullptr)
        return ERR_CTRLAPI_INPUTERR;

    /***************************组装数据**************************/

    bufLen = GetCmdLen(cpBytes);

    //数据头 + 数据
    if (wtBuf != nullptr)//是否使用栈数据优化过
    {
        if (bufSize < bufLen)
            return ERR_CTRLAPI_MEMSMALL;
        sndBuff = wtBuf;
    }
    else
    {
        sndBuff = new byte[bufLen];
        if (sndBuff == nullptr)
            return ERR_CTRLAPI_MEMALLOC;
    }

    //设置内容
    SetCmdContent(param, 1, sndBuff, bufLen);

    //数据复制
    if (cpBytes > 0)
        memcpy(sndBuff + sizeof(QnA3ECmd), val, cpBytes);

    /******设置反馈用内存*******/

    //是需要再申请内存来存放ACK，或者直接用发送缓冲的数据
    ackLen = sizeof(tagQnA3EAck);
    if (ackLen <= bufLen)
        ackBuf = sndBuff;
    else//如果空间不够
    {
        if (wtBuf != nullptr)//是否使用栈数据优化过
        {
            if (ackLen > bufSize)
                return ERR_CTRLAPI_MEMSMALL;
            else
                ackBuf = sndBuff;
        }
        else
        {
            ackBuf = new byte[ackLen];
            if (ackBuf == nullptr)
            {
                delete[] sndBuff;
                return ERR_CTRLAPI_MEMALLOC;
            }
        }
    }

    /***********************发送处理反馈*************************/

    //发送数据
    ret = SendAndRecv(sndBuff, bufLen, ackBuf, ackLen);
    if (ret != ERR_CTRLAPI_NOERR)
        goto labEnd;

    //检查反馈值（写入的反馈长度一般为2字节，即为错误码）
    if ((ret = CheckAck(ackBuf, sizeof(uint16))) != ERR_CTRLAPI_NOERR)
        goto labEnd;

    //是否需要释放
    if (wtBuf == nullptr)//是否使用栈数据优化过
    {
        if (ackBuf != sndBuff)	delete[] ackBuf;
        delete[] sndBuff;
    }

    return ERR_CTRLAPI_NOERR;

labEnd:
    if (wtBuf == nullptr)//是否使用栈数据优化过
    {
        if (ackBuf != sndBuff)	delete[] ackBuf;
        delete[] sndBuff;
    }
    return ret;
}

int McClient::BulkRead(const PLCParam &param, byte **buff, int *len, byte *stBuf, int bufSize)
{
    QnA3ECmd cmd;
    int ret, ackLen, cpBytes;
    byte *ackBuf = nullptr;

    //获取能读取出的字节长度
    cpBytes = GetBulkWDataLen(param);
    ackLen = cpBytes + sizeof(tagQnA3EAck);

    if (stBuf != nullptr)
    {
        if (bufSize < ackLen)
            return ERR_CTRLAPI_MEMSMALL;
        ackBuf = stBuf;
    }
    else
        ackBuf = new byte[ackLen];

    if (ackBuf == nullptr)
        return ERR_CTRLAPI_MEMALLOC;

    /***************************组装数据**************************/
    //设置内容
    SetCmdContent(param, 0, (byte *)&cmd, sizeof(cmd));

    //发送数据
    ret = SendAndRecv((byte *)&cmd, sizeof(cmd), ackBuf, ackLen);
    if (ret != ERR_CTRLAPI_NOERR)
        goto labEnd;

    //检查反馈值（2为错误码长度 + 数据长度）
    if ((ret = CheckAck(ackBuf, cpBytes + sizeof(uint16))) != ERR_CTRLAPI_NOERR)
        goto labEnd;

    *buff = ackBuf;
    *len = ackLen;
    return ERR_CTRLAPI_NOERR;

labEnd:
    if (stBuf == nullptr)
        delete[] ackBuf;
    return ret;
}

int McClient::WriteBit(PlcMemType type, uint32 stAddr, byte val)
{
    PLCParam param;
    byte buf[sizeof(QnA3ECmd) + 1];//22

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTBit;
    //点数
    param.Count = 1;

    //从高位开始表示
    //两个点一个字节
    val = (val != 0) ? 0x10 : 0x0;

    //发送数据
    return BulkWrite(param, &val, 1, buf, sizeof(QnA3ECmd) + 1);
}

int McClient::WriteWord(PlcMemType type, uint32 stAddr, uint16 val)
{
    PLCParam param;
    byte buf[sizeof(QnA3ECmd) + sizeof(uint16)];//25

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTWord;
    //点数
    param.Count = 1;

    //发送数据
    //小端传送
    return BulkWrite(param, (byte*)&val, sizeof(uint16), buf, sizeof(QnA3ECmd) + sizeof(uint16));
}

int McClient::WriteDouleWords(PlcMemType type, uint32 stAddr, uint32 val)
{
    PLCParam param;
    byte buf[sizeof(QnA3ECmd) + sizeof(uint32)];//25

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTWord;
    //点数
    param.Count = 2;

    //发送数据
    //小端传送
    return BulkWrite(param, (byte*)&val, sizeof(uint32), buf, sizeof(QnA3ECmd) + sizeof(uint32));
}

int McClient::ReadBit(PlcMemType type, uint32 stAddr, byte &val)
{
    PLCParam param;
    int ret, ackLen;
    byte *buf, *data;
    byte stBuf[sizeof(tagQnA3EAck) + 1];//12

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTBit;
    //点数
    param.Count = 1;

    ret = BulkRead(param, &buf, &ackLen, stBuf, sizeof(tagQnA3EAck) + 1);
    if (ret != ERR_CTRLAPI_NOERR)
        return ret;

    //解析数据
    data = buf + sizeof(tagQnA3EAck);

    //从高位开始表示
    //两个点一个字节
    //0x10 : 0x0
    val = (*data == 0x00) ? 0 : 1;

    return ERR_CTRLAPI_NOERR;
}

int McClient::ReadWord(PlcMemType type, uint32 stAddr, uint16 &val)
{
    PLCParam param;
    int ret, ackLen;
    byte *buf;
    byte stBuf[sizeof(tagQnA3EAck) + sizeof(uint16)];//15

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTWord;
    //点数
    param.Count = 1;
    //小端传送
    ret = BulkRead(param, &buf, &ackLen, stBuf, sizeof(tagQnA3EAck) + sizeof(uint16));
    if (ret != ERR_CTRLAPI_NOERR)
        return ret;

    //解析数据
    val = *(uint32*)(buf + sizeof(tagQnA3EAck));

    return ERR_CTRLAPI_NOERR;
}

int McClient::ReadDouleWords(PlcMemType type, uint32 stAddr, uint32 &val)
{
    PLCParam param;
    int ret, ackLen;
    byte *buf;
    byte stBuf[sizeof(tagQnA3EAck) + sizeof(uint32)];//15

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTWord;
    //点数
    param.Count = 2;
    //小端传送
    ret = BulkRead(param, &buf, &ackLen, stBuf, sizeof(tagQnA3EAck) + sizeof(uint32));
    if (ret != ERR_CTRLAPI_NOERR)
        return ret;

    //解析数据
    val = *(uint32*)(buf + sizeof(tagQnA3EAck));

    return ERR_CTRLAPI_NOERR;
}

int McClient::BulkReadWord(PlcMemType type, uint32 stAddr, int stCnt, byte **val, int *dataOffset, int *num)
{
    PLCParam param;
    int ret, ackLen;
    byte *buf;

    if (num == nullptr)
        return ERR_CTRLAPI_INPUTERR;

    //起始地址
    param.StartAddr = stAddr;
    //内存类型
    param.MemType = type;
    //PMemDR只能按字获取
    param.DataType = OPUTWord;
    //点数
    param.Count = stCnt;
    //小端传送
    ret = BulkRead(param, &buf, &ackLen);
    if (ret != ERR_CTRLAPI_NOERR)
        return ret;

    //解析数据
    *val = buf;
    *dataOffset = sizeof(tagQnA3EAck);
    *num = ackLen;

    //delete[]buf;由调用者释放内存
    return ERR_CTRLAPI_NOERR;
}

int McClient::Connect(QString ipAddress, uint16 port)
{
    this->connectToHost(QHostAddress(ipAddress),port);
    m_isOpen = this->waitForConnected(1000);
    return m_isOpen==true? 0:-1;
}
