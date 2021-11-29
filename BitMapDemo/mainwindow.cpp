#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QImage image("C:\\Users\\PL1000\\Desktop\\10.bmp");
    qDebug()<<"imageDepth:"<<image.depth();
    int depth = image.depth();
    qDebug()<<"fromat :"<<image.format();
    qDebug()<<"Width"<<image.width()<<"Heithg:"<<image.height();
    qDebug()<<"bytesPerLine:"<<image.bytesPerLine();
    qDebug()<<"byteCount: "<<image.sizeInBytes();
    int bytePos = image.bytesPerLine();// per byte count
    for(int x = 0; x < image.height(); x++)
    {
        //char_k *k_char = reinterpret_cast<char_k *>(image.scanLine(x));
        uchar *k_char = image.scanLine(x);
        QVector<int> vec;
        vec.clear();
        bytePos = bytePos * x;
        for(int y = 0; y <image.width(); y++)
        {
            char v = 1;
            if(depth == 1)
                 v = k_char[( y/8)] >> ( 7- (y %8)) & 0x01;
            else if(depth == 4)
                 v = k_char[( y/2)] >> ( 7- (y %8)) & 0x01;
           vec.append((int)(v));
        }
        qDebug()<<vec;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
