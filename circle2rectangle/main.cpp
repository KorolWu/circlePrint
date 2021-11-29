#include "mainwindow.h"

#include <QApplication>
#include "opencv2/opencv.hpp"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/ico/kimage.png"));
    w.show();

//    Mat src = imread("C:\\Users\\jian.shen\\Pictures\\newGrey.png");
//    qDebug()<<"newGrey channels:"<<src.channels();
//    Mat grey;
//    cvtColor(src,grey,COLOR_BGR2GRAY);
//    qDebug()<<"grey channels:"<<grey.channels();
//    imwrite("C:\\Users\\jian.shen\\Pictures\\opencvGrey.png",grey);

    return a.exec();
}
