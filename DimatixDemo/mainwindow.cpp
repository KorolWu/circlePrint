#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);//0.58   init wave befor print
    //----init mc-------
    m_pMcClientHelper =  new McClientHelper();

    int connectStauts =  Print_Connect(0xC0A80372,1234);
    if(connectStauts == 0)
    {
        ui->textEdit->append(QString("connection success!").arg(connectStauts));
        connectStauts =  Print_PreInit();
        if(connectStauts == 0)
        {
            m_canStart = true;
            ui->textEdit->append("power init success");
        }

    }
    else
    {
        ui->textEdit->append(QString("connection failed errCode %1").arg(connectStauts));
    }
    this->setStyleSheet("QPushButton{background-color:rgb(140,140,150);border-radius: 5px;font:12px;color:white}QPushButton:hover{background-color:rgb(210,210,210);}QPushButton:pressed{background-color:rgb(160,160,160);}QLabel{font: 14px;}QLineEdit,QComboBox,QDoubleSpinBox,QSpinBox{Padding-right:20px;Border:2px solid white;font:20px;Min-height:15px;border-radius:5px;}QDoubleSpinBox::up-button,QDoubleSpinBox::down-button,QSpinBox::up-button,QSpinBox::down-button{ width:0px;}QGroupBox{background-color:rgb(245,245,240);border: 2px solid white;border-radius:8px;margin-top:6px;}QGroupBox:title{color:rgb(24,24,58);subcontrol-origin: margin;left: 10px;}QGroupBox{font: 20px;}");
    m_pSplitCfg = new SwatheSplitCfg();
    //init status timer
    ui->status_lab->setVisible(false);
    m_timer = new QTimer;
    connect(m_timer,&QTimer::timeout,this,&MainWindow::onTimeout);
    ui->statusbar->setVisible(false);


}

MainWindow::~MainWindow()
{
    m_pMcTimer->stop();
    Print_Disconnect();
//    if(m_pMcClientHelper != nullptr)
//    {
//        m_pMcClientHelper->disConnect();
//        m_pMcClientHelper = nullptr;
//        delete m_pMcClientHelper;
//    }
    delete ui;
}

//set parameter
void MainWindow::on_pushButton_clicked()
{
    int re = -1;
    if(m_waveIsSet == false)
    {
        int upDownTime = ui->upDownTime->value();
        int holdTime = ui->holdTime->value();
        int vol = ui->Vol->value();
        WaveformInfo *waveInfo = new WaveformInfo();
        waveInfo->Rise = static_cast<unsigned char>(upDownTime);
        waveInfo->Fall = static_cast<unsigned char>(upDownTime);
        waveInfo->Due  = static_cast<unsigned char>(holdTime);
        waveInfo->Vol  = static_cast<unsigned char>(vol);
        waveInfo->ChnAddr = WFChnAddr::WFChnAddr1;
        re = Print_SetWaveform(waveInfo);
        if(re == 0)
        {
            ui->textEdit->append("SetWaveform Ok!");
            m_waveIsSet = true;
        }
        else {
            ui->textEdit->append("SetWaveform failed");
        }
    }
    //    qDebug()<<waveInfo->Rise<<waveInfo->Fall<<waveInfo->Due<<waveInfo->Vol<<waveInfo->ChnAddr;
    // set print cfg
    m_cfg.EncoderRes  = static_cast<float>(ui->encodeSpinBox->value()) ;
    m_cfg.RequiredDPI = ui->yDpi->value();
    re = Print_SetPrintCfg(&m_cfg);

    m_pSplitCfg->StartNoz =static_cast<unsigned int>(ui->starNol->value()) ;
    m_pSplitCfg->EndNoz   = static_cast<unsigned int>(ui->endNol->value());
    m_pSplitCfg->PrintDPI = ui->xDpi->value(); //x dir
    m_pSplitCfg->PHType   = PHType::PHTQS256;




    Print_SetPHType(PHType::PHTQS256);
    //设置工作频率
    Print_SetFireFreq(20);
    //设置波形
    re = Print_Init(PHType::PHTQS256);
    if(re != 0)
        ui->textEdit->append("Print_SetPrintCfg failed");
    else {
        ui->textEdit->append("SetPrintCfg Ok!");
    }

//--------------set plc pamater && connect------
    if(m_pMcClientHelper != nullptr)
    {
         m_mcIsConnect = m_pMcClientHelper->Connect(QString("192.168.3.250"),int(8000));
         qDebug()<<m_mcIsConnect;
    }
    m_mcIsConnect == true ? ui->connectStatus->setStyleSheet("background-color: rgb(0,255,0);"):ui->connectStatus->setStyleSheet("background-color: rgb(255, 0, 0);");
    m_pMcTimer = new QTimer(this);
    connect(m_pMcTimer,&QTimer::timeout,this,&MainWindow::onMcTimerTimout);
    m_pMcTimer->start(100);
}

// Select image
void MainWindow::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("文件对话框！"),
                                                    "F:",
                                                    tr("图片文件(*bmp);;"));
    if(fileName == "")
        return;
    m_path  = fileName;
    ui->pathEdit->setText(m_path);

}

void MainWindow::on_startbtn_clicked()
{
    QByteArray buff = m_path.toLatin1();
    if(buff.size() == 0)
    {
        ui->textEdit->append("path does not exist");
        return;
    }
    char *path = buff.data();
    m_print = new PrintInfo();
    wchar_t w_path[64] = {'\0'};
    MBCS2WCS(path,w_path,64);


    if(m_pSplitCfg == nullptr|| m_print == nullptr)
        return;
    QString savePath = "D:";
    QByteArray saveBuff = savePath.toLatin1();
    int re = GenSwatheInfoFromFile(w_path,m_pSplitCfg,&m_print);
    // int re = GenSwatheInfoFromFile_korol(path,m_pSplitCfg,&m_print);
    saveSplitImage(m_print);
    if(re != 0)
    {
        ui->textEdit->append("GenSwatheInfoFromFile failed");
        return;
    }
    ui->bigPass->setValue(m_print->PassNum);
    ui->smallPass->setValue(m_print->StepNum);

//    re = Print_DownloadSwathe(0,*m_print->SwatheInfoSet,1);
//    if(re != 0)
//    {
//        ui->textEdit->append("Download Swathe failed,try again");
//        return;
//    }
    bool isMoveDown = move_y(436);
    if(isMoveDown == false)
    {
        ui->textEdit->append("Y Axis Move failed");
        return;
    }
   isMoveDown =  move_x(433);
   if(isMoveDown == false)
   {
       ui->textEdit->append("X Axis Move failed");
       return;
   }
   isMoveDown = move_u(902);
   if(isMoveDown == false)
   {
       ui->textEdit->append("U Axis Move failed");
       return;
   }
    //---------将x和圆盘移动到待打印位置-----


        ui->textEdit->append("Print Start!");
        m_timer->setInterval(1000);
        m_timer->start();
        //开始喷
        for(int count = 0; count < m_print->PassNum; count ++)
        {
            //re = Print_DownloadSwathe(0,*m_print->SwatheInfoSet,1);
            re = Print_DownloadSwathe(0,m_print->SwatheInfoSet[count],1);
            if(re != 0)
            {
                ui->textEdit->append("Download Swathe failed,try again");
                return;
            }

            re =  Print_EnterPrintMode();
            if(re != 0 )
            {
                ui->textEdit->append("enterPrintMode failed");
                return;
            }
            isMoveDown = move_u(920);
            if(isMoveDown == false)
            {
                ui->textEdit->append("U Axis Move failed");
                return;
            }
            Print_ExitPrintMode();
            isMoveDown = move_u(902);
            if(isMoveDown == false)
            {
                ui->textEdit->append("U Axis Move ORG failed");
                return;
            }
            if(count != m_print->PassNum -1)
            {
                isMoveDown = move_y(437);
                if(isMoveDown == false)
                {
                    ui->textEdit->append("Y Axis Move failed");
                    return;
                }
            }
            sleep(500);
        }
        //结束 x 轴回到打印初始位置
        isMoveDown =  move_x(602);
        if(isMoveDown == false)
        {
            ui->textEdit->append("X Axis Move failed");
            return;
        }
        Print_ExitPrintMode();
        ui->textEdit->append("end print");
        m_timer->stop();
        ui->status_lab->setVisible(false);

}

void MainWindow::on_endBtn_clicked()
{
    //end print
    Print_ExitPrintMode();
    ui->textEdit->append("end print");
    m_timer->stop();
    ui->status_lab->setVisible(false);
}

void MainWindow::on_pushButton_3_clicked()
{

    if(m_canStart)
    {
        int re = Print_StartSpitting(200,1,1);
        if(re == 0)
        {
            m_timer->setInterval(50);
            m_timer->start();
            ui->textEdit->append("Start Spitting");

        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->status_lab->setVisible(false);
    if(m_canStart)
    {
        int re =  Print_StopSpitting();
        m_timer->stop();
        if(re == 0)
            ui->textEdit->append("End Spitting");
    }
}

void MainWindow::onTimeout()
{
    static bool status = false;
    status = !status;
    if(status)
    {
        ui->status_lab->setVisible(true);
    }
    else
        ui->status_lab->setVisible(false);

}

void MainWindow::on_noTypecomboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    QString typeStr = ui->noTypecomboBox->currentText();
    typeStr == "128" ? m_type = PHType::PHTSE128 : m_type = PHType::PHTQS256;
    qDebug()<<"PHtype:"<<typeStr;
    Print_SetPHType(m_type);
    //设置工作频率
    Print_SetFireFreq(2561);
    m_pSplitCfg->PHType = m_type;
    //设置波形
    int re = Print_Init(m_type);
    if(re == 0)
        ui->textEdit->append("Init success");

}

void MainWindow::saveSplitImage(PrintInfo *swatheInfo)
{
    if(swatheInfo == nullptr)
        return;
    if(swatheInfo->PassNum == 0)
        return;
    QImage *image = nullptr;
    for(int i = 0; i<swatheInfo->SwatheCount; i++)
    {
        image = new QImage((swatheInfo->SwatheInfoSet[i])->Data,(swatheInfo->SwatheInfoSet[i])->Width,(swatheInfo->SwatheInfoSet[i])->Height,QImage::Format_Indexed8);
        image->save(QString("D:\\splite\\image%1.png").arg(i));
        //*swatheInfo->SwatheInfoSet++;
    }
}

///
/// \brief MainWindow::GenSwatheInfoFromFile_korol
/// \param file
/// \param cfg
/// \param printInfo
/// \return
///
int MainWindow::GenSwatheInfoFromFile_korol(const char *file, const SwatheSplitCfg *cfg, PrintInfo **printInfo)
{

    if(file == nullptr || cfg ==nullptr || *printInfo == nullptr)
        return -1;
    QString path(file);
    QImage src(path);
    if(src.depth() != 1)  // depth != 1
        return -2;
    float dstDpi = cfg->PrintDPI;  //dst dpi
    float srcDpi = 100;            //src dpi
    uint pass = static_cast<uint>(dstDpi / srcDpi);
    int width = src.width();
    uint bigPass = (0.5+width /((cfg->EndNoz - cfg->StartNoz +1))) + (width % ((cfg->EndNoz - cfg->StartNoz +1)) == 0 ? 0 : 1);
    qDebug()<<"bigPass "<<bigPass<<" smallPass"<<pass;
    int bigPassCount = 0;
    int passCuont = 0;
    SwatheInfo ** swSet = nullptr;

    swSet = new SwatheInfo *[bigPass *pass];
    uint relayNoz = (cfg->EndNoz - cfg->StartNoz +1) * static_cast<uint>(src.height());
    for(uint i = 0; i < bigPass; i++) //big pass
    {
        uchar *passBuff = new uchar[relayNoz];

        for(uint j = 0; j< pass; j++) //small pass
        {
            for(uint x = 0; x < 256; x ++)  //can use noz
            {
                if(x >= cfg->StartNoz && x <= cfg->EndNoz)
                {
                    uint temp_x = x+ (cfg->EndNoz - cfg->StartNoz +1)*i + pass*j;  //witch bit in x
                    qDebug()<<temp_x;
                    for(uint y = 0; y < static_cast<uint>(src.height()); y++) //image height
                    {
                        if(y > static_cast<uint>(src.height())) continue;
                        uchar * linePer = src.scanLine(static_cast<int>(y));
                        uchar v =  (linePer[temp_x/8] >> (7- (temp_x %8))) & 0x01;//linePer[(temp_x+(y*src.width()))/8] >> ((7- ((temp_x+(y*src.width())) %8)) & 0x01);
                        if(temp_x >= static_cast<uint>(src.width()))
                            passBuff[x+(y*(cfg->EndNoz - cfg->StartNoz +1))]=255;
                        else
                            passBuff[x+(y*(cfg->EndNoz - cfg->StartNoz +1))] = (v == 0? 255 : 0); //---- ? no pass in bigpass
                        if((y*(cfg->EndNoz - cfg->StartNoz +1))  >= relayNoz)
                            qDebug()<<"array ++";
                        //qDebug()<<temp_x+(y*(cfg->EndNoz - cfg->StartNoz +1));
                    }
                }
            }
            // qDebug()<<src.format();
            bigPassCount ++;
            passCuont++;
        }
        SwatheInfo *sw = new SwatheInfo();
        sw->Width = width;
        sw->Height = src.height();
        //一个像素占用的比特数1,2,4，8bit
        sw->BitsPerPixel = 8;//HS版只支持单色位图
        //图像缓冲
        //sw->Data = passBuff;
        sw->Data = new uchar[relayNoz];
        memcpy(sw->Data,passBuff,src.width()*src.height());
        //数据长度（字节数）
        sw->Length =src.width()*src.height();
        sw->PHType = PHType::PHTQS256;

        swSet[i] = sw;

        m_dst1 = new QImage(passBuff,static_cast<int>(cfg->EndNoz - cfg->StartNoz +1),src.height(),QImage::Format_Indexed8);
        m_dst1->save(QString("D:\\splite\\bigPass%1.png").arg(i));
        //生成 SwatheInfo



        //memcpy((*(*printInfo)->SwatheInfoSet)->Data,passBuff,relayNoz);
//        (*(*printInfo)->SwatheInfoSet)->Data = passBuff;
//        (*(*printInfo)->SwatheInfoSet)->Width = width;
//        (*(*printInfo)->SwatheInfoSet)->Height = src.height();
//        (*(*printInfo)->SwatheInfoSet)->Length = relayNoz;
//        (*(*printInfo)->SwatheInfoSet)->PHType = PHType::PHTQS256;
//        (*(*printInfo)->SwatheInfoSet)->BitsPerPixel = src.depth();
//        (*(*printInfo)->SwatheInfoSet)++;
//        (*printInfo)->SwatheCount++;
//        (*printInfo)->PassNum = i;
//        (*printInfo)->StepNum = pass;

        *printInfo = new PrintInfo;
        (*printInfo)->SwatheCount = pass*bigPass;
        (*printInfo)->SwatheInfoSet = swSet;
        (*printInfo)->StepNum = bigPass;
        (*printInfo)->PassNum = pass;
        memset(passBuff,0,relayNoz);
        passBuff = nullptr;
        delete[] passBuff;
        m_dst1 = nullptr;
        delete m_dst1;
    }
    return 1;
}

void MainWindow::testGenSwatheInfoFromFile_korolFunc(const SwatheSplitCfg *cfg,PrintInfo **printInfo)
{
    QString path = "C:\\Users\\PL1000\\Desktop\\aa.bmp";
    QByteArray buff = path.toLatin1();
    GenSwatheInfoFromFile_korol(buff.data(),cfg,printInfo);
}

///
/// \brief MainWindow::onMcTimerTimout 刷新M 和 D寄存器
///
void MainWindow::onMcTimerTimout()
{
    int socketok = 0;
    if (m_mcIsConnect)//已连接
    {
        //-----------读取所有信息；
        socketok = m_pMcClientHelper->read_mulit_words(D, 400, 100);
        socketok = m_pMcClientHelper->read_mulit_words(M, 400, 36);
        //x 位置
        int xPos = m_pMcClientHelper->plc_d[402] | m_pMcClientHelper->plc_d[403] << 16;
        ui->xPos->setText(QString::number(xPos));
        int yPos = m_pMcClientHelper->plc_d[406] | m_pMcClientHelper->plc_d[407] << 16;
        ui->yPos->setText(QString::number(yPos));
        int zPos = m_pMcClientHelper->plc_d[410] | m_pMcClientHelper->plc_d[411] << 16;
        ui->uPos->setText(QString::number(zPos));
        m_mcIsConnect = socketok;
        //qDebug()<<"x is move:"<<m_pMcClientHelper->plc_m[480];
    }
    else
    {
        ui->connectStatus->setStyleSheet("background-color: rgb(255, 0, 0);");
        m_mcIsConnect = false;

    }
}

void MainWindow::sleep(int time)
{
    QEventLoop loop;
    QTimer::singleShot(time,&loop,SLOT(quit()));
    loop.exec();
}

///
/// \brief MainWindow::move_x
/// \param port 432打印初始位置   433打印终点位置
/// \return
///
bool MainWindow::move_x(int port)
{
    m_pMcClientHelper->write_byte(M,port,1);
    bool isMoveDown = m_pMcClientHelper->Ismove_done(X);
    return isMoveDown;
}

///
/// \brief MainWindow::move_u
/// \param port 920 终点位置   902原点
/// \return
///
bool MainWindow::move_u(int port)
{
    m_pMcClientHelper->write_byte(M,port,1);
    QThread::msleep(300);
    bool isMoveDown = m_pMcClientHelper->Ismove_done(U);
    return isMoveDown;
}

///
/// \brief MainWindow::move_y
/// \param port 436 打印初始位置   437微动增量位置
/// \return
///
bool MainWindow::move_y(int port)
{
    m_pMcClientHelper->write_byte(M,port,1);
    QThread::msleep(300);
    bool isMoveDown = m_pMcClientHelper->Ismove_done(Y);
    return isMoveDown;
}

///
/// \brief MainWindow::on_x_add_pressed  X+
///
void MainWindow::on_x_add_pressed()
{
    m_pMcClientHelper->write_byte(M,430,1);
}

///
/// \brief MainWindow::on_x_dcc_released X-
///
void MainWindow::on_x_dcc_released()
{
    m_pMcClientHelper->write_byte(M,431,0);
}

///
/// \brief MainWindow::on_x_add_released X+ stop
///
void MainWindow::on_x_add_released()
{
     m_pMcClientHelper->write_byte(M,430,0);
}

///
/// \brief MainWindow::on_x_dcc_pressed X- stop
///
void MainWindow::on_x_dcc_pressed()
{
    m_pMcClientHelper->write_byte(M,431,1);
}

////
/// \brief MainWindow::on_x_start_clicked x轴打印初始位置
///
void MainWindow::on_x_start_clicked()
{
//    m_pMcClientHelper->write_byte(M,432,1);
//    bool isMoveDown = m_pMcClientHelper->Ismove_done(X);
//    if(isMoveDown)
//        ui->textEdit->append("Axis initPos success");
    //----------------U ORG--------------

        m_pMcClientHelper->write_byte(M,902,1);
        QThread::msleep(300);
        bool isMoveDown = m_pMcClientHelper->Ismove_done(U);
        if(isMoveDown)
            ui->textEdit->append("U Axis ORG success");

}

void MainWindow::on_x_end_clicked()
{
//    m_pMcClientHelper->write_byte(M,433,1);
//    bool isMoveDown = m_pMcClientHelper->Ismove_done(X);
//    if(isMoveDown)
//        ui->textEdit->append("Axis initPos success");
    //----------------U endPos--------------
    m_pMcClientHelper->write_byte(M,920,1);
    QThread::msleep(300);
    bool isMoveDown = m_pMcClientHelper->Ismove_done(U);
    if(isMoveDown)
        ui->textEdit->append("U Axis endPos success");
}
