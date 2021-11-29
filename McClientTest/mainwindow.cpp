#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ipAddress("192.168.3.250")
    , m_port(8000)
    , ui(new Ui::MainWindow)
    , m_connectStatus(false)
{
    ui->setupUi(this);
    m_pMcHelper = new McClientHelper();
    m_connectStatus = m_pMcHelper->Connect(m_ipAddress,m_port);
    if(true == m_connectStatus)
        qDebug()<<"connect success";

    m_pTimer = new QTimer(this);
    connect(m_pTimer,&QTimer::timeout,this,&MainWindow::onPlcTimerOut);

    m_pTimer->start(10);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPlcTimerOut()
{
    int socketok = 0;
    if (m_connectStatus)//已连接
    {
        //-----------读取所有信息；
        socketok = m_pMcHelper->read_mulit_words(D, 400, 100);
        socketok = m_pMcHelper->read_mulit_words(M, 400, 7);
        //x 位置
        int xPos = m_pMcHelper->plc_d[402] | m_pMcHelper->plc_d[403] << 16;
        ui->XlineEdit->setText(QString::number(xPos));
        int yPos = m_pMcHelper->plc_d[406] | m_pMcHelper->plc_d[407] << 16;
        ui->YlineEdit->setText(QString::number(yPos));
        int zPos = m_pMcHelper->plc_d[410] | m_pMcHelper->plc_d[411] << 16;
        ui->ZlineEdit->setText(QString::number(zPos));
        m_connectStatus = socketok;
        // g_3D_Error[25] = !socketok;
        //ui->XlineEdit
    }
    else
    {
//        if (!m_connectStatus)//已连接
//            m_connectStatus = m_pMcHelper->Connect(m_ipAddress,m_port);

        //g_3D_Error[25] = !isCon;

    }
}

