#include "mcclienthelper.h"

McClientHelper::McClientHelper()
{

    m_pMcClientHandle = new McClient();
    memset(plc_d,0,sizeof (plc_d));
    memset(plc_m,0,sizeof (plc_m));
}

McClientHelper::~McClientHelper()
{
    if(m_pMcClientHandle != nullptr)
    {
        //再此之前最好调用 socket的断开连接
        m_pMcClientHandle = nullptr;
        delete m_pMcClientHandle;
    }
}

bool McClientHelper::Connect(QString IPAddress, uint16_t PortNum)
{
    return (0 == m_pMcClientHandle->Connect(IPAddress,PortNum));
}

bool McClientHelper::write_byte(Type type, int port, int value)
{
   // return 0 == PLC_WriteBit(m_plcHandle, PMemMR, (uint32)port, (value == 1) ? 1 : 0);
    return 0 == m_pMcClientHandle->WriteBit(PMemMR, (uint32)port, (value == 1) ? 1 : 0);
}

bool McClientHelper::write_2_word(Type type, int port, int val)
{
    //return  0 == PLC_WriteDouleWords(m_plcHandle, PMemDR, (uint32)port, (uint32)val);
    return 0 == m_pMcClientHandle->WriteDouleWords(PMemDR, (uint32)port, (uint32)val);

        //PLCParam param;
        //byte data;
        //byte buf[sizeof(tagQnA3EAck) + sizeof(val)];//保证足够大的缓冲区

        ////起始地址
        //param.StartAddr = (uint32)port;
        ////内存类型
        //param.MemType = PMemDR;
        ////PMemDR只能按字获取
        //param.DataType = OPUTWord;
        ////点数
        //param.Count = 2;


        //return 0 == PLC_BulkWrite(m_plcHandle, param, ((byte *)&val), sizeof(val), buf, sizeof(tagQnA3EAck) + sizeof(val));
}

bool McClientHelper::read_mulit_words(Type type, int port, int words_num)
{
    PlcMemType menType;
        //----------------类型
        switch (type) {
        case D:
        default:
            menType = PMemDR;// D_SEG;
            break;
        case M:
            menType = PMemMR;
            break;
        }

        //1.先刷新定位结果
        int ret, len;
        PLCParam param;
        byte *ref, *ack;
        byte buf[1024];

        //起始地址
        param.StartAddr = (uint32)port;
        //内存类型
        param.MemType = menType;
        //PMemDR只能按字获取
        param.DataType = OPUTWord;
        //点数
        param.Count = words_num;

        //先读取数据
        if (0 != (ret = m_pMcClientHandle->BulkRead(param, &ack, &len, buf, 1024)))
            return false;

        ref = (ack + sizeof(tagQnA3EAck));

        for (int word_count = 0; word_count < words_num; word_count++)
        {
            if (type == M)
            {
                for (int k = 0; k < 8; k++)
                {
                    plc_m[port + k + word_count * 2 * 8] = (int)((unsigned char)ref[word_count * 2]) >> k & 0x01;
                }
                for (int k = 8; k < 16; k++)
                {
                    plc_m[port + k + word_count * 2 * 8] = (int)((unsigned char)ref[word_count * 2 + 1]) >> (k - 8) & 0x01;
                }
            }
            else if (type == D)
            {
                plc_d[port + word_count] = (int)((unsigned char)ref[word_count * 2]) | (int)((unsigned char)ref[word_count * 2 + 1]) << 8;
            }
        }

        return true;
}

bool McClientHelper::Ismove_done(int motornum, int waittime)
{
    QThread::msleep(150);
        int count = 0;
        bool mok = false;
        switch (motornum) {
        case X:
            while (count < waittime)
            {
                if (!plc_m[480])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
            break;
        case Y:
            while (count < waittime)
            {
                if (!plc_m[481])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
            break;
        case Z:
            while (count < waittime)
            {
                if (!plc_m[482])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
            break;
        case GD:
            while (count < waittime)
            {
                if (!plc_m[483])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
            break;
        case GT:
            while (count < waittime)
            {
                if (!plc_m[484])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
            break;
        case FY:
            while (count < waittime)
            {
                if (!plc_m[485])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
            break;
        case XORG:
            while (count < waittime)
            {
                if (!plc_m[603])
                {
                    mok = true;
                    break;
                }
                QThread::msleep(1);
            }
        }
        return mok;
}

bool McClientHelper::cmppos(int motornum, int port)
{
    bool same = false;
        QThread::msleep(100);
        switch (motornum) {
        case X:
            if (plc_d[port] == plc_d[402] && plc_d[port + 1] == plc_d[403])
                same = true;
            break;
        case Y:
            if (plc_d[port] == plc_d[406] && plc_d[port + 1] == plc_d[407])
                same = true;
            break;
        case Z:
            if (plc_d[port] == plc_d[410] && plc_d[port + 1] == plc_d[411])
                same = true;
            break;
        case GD:
            if (plc_d[port] == plc_d[414] && plc_d[port + 1] == plc_d[415])
                same = true;
            break;
        case GT:
            if (plc_d[port] == plc_d[418] && plc_d[port + 1] == plc_d[419])
                same = true;
            break;
        case FY:
            if (plc_d[port] == plc_d[424] && plc_d[port + 1] == plc_d[425])
                same = true;
            break;
        case XSTART:
            if (plc_d[430] == plc_d[402] && plc_d[430 + 1] == plc_d[403])
                same = true;
            break;
        }
        return same;
}

bool McClientHelper::LockTopCover(bool lock)
{
    return write_byte(M, 422, lock ? 1 : 0);
}

bool McClientHelper::SetRoller(bool isOn)
{
    return write_byte(M, 448, isOn ? 1 : 0);
}
