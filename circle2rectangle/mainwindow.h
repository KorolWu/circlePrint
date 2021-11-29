#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include "opencv2/opencv.hpp"
#include <QTimer>
#include <qmath.h>
#include <math.h>


#define PI 3.1415926
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    bool cartesian_to_polar(cv::Mat& mat_c, cv::Mat& mat_p, int img_d);
    bool polar_to_cartesian(cv::Mat& mat_p, cv::Mat& mat_c, int rows_c, int cols_c);
    bool polar_to_cartesian_V1(cv::Mat &src, cv::Mat &dst);
    uchar interpolate_bilinear(cv::Mat& mat_src, double ri, int rf, int rc, double ti, int tf, int tc);
    void onTimeOut();
    void polar2rectangle(int r,double angle);

    void showPixValue(cv::Mat& src);
    double angle(QPoint center, int y, int x);
    bool isInCricle(QPoint center,int R, int y, int x, double &r);


private:
    Ui::MainWindow *ui;
    QString m_path;
    QTimer *m_pTimer;
    int m_x = -1,m_y=-1;
};
#endif // MAINWINDOW_H
