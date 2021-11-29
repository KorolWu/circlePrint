#ifndef MCCLIENTHELPER_H
#define MCCLIENTHELPER_H

#include "mcclient.h"
#include "PLCMCQnA3E.h"
#include <QThread>
#include <QEventLoop>
#include <QTimer>

enum Type { D, M, L };
enum Motor { X, Y, Z, GD, GT, FY, XORG, XSTART, U };
class McClientHelper
{
private:
    McClient *m_pMcClientHandle;
public:
    McClientHelper();
    ~McClientHelper();
public:
    //----------------------M位状态
    bool plc_m[1000];
    //----------------------d状态
    int plc_d[1000];

public:
    bool Connect(QString IPAddress, uint16_t PortNum);
    void disConnect();
    //-------读写指令
    bool write_byte(Type type, int port, int value);
    bool write_2_word(Type type, int port, int val);// int value1, int value2);

    bool read_mulit_words(Type type, int port, int words_num);

    //-----------等待电机信号 运动完成

    bool Ismove_done(int motornum, int waittime = 1000000);
    //-----
    bool cmppos(int motornum, int port = 0);


    //---------------------------------

    bool LockTopCover(bool lock);


    //开启滚筒电机
    bool SetRoller(bool isOn);

    void sleep(int time);
};

#endif // MCCLIENTHELPER_H
