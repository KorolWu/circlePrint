#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;
using namespace cv;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pTimer = new QTimer(this);
    connect(m_pTimer,&QTimer::timeout,this,&MainWindow::onTimeOut);
    this->setStyleSheet("QPushButton{background-color:rgb(140,140,150);border-radius: 5px;font:16px;color:white;Min-height:30px;Max-width:80px}QPushButton:hover{background-color:rgb(210,210,210);}QPushButton:pressed{background-color:rgb(160,160,160);}QLabel{font: 14px;}QLineEdit,QComboBox,QDoubleSpinBox,QSpinBox{Padding-right:20px;Border:2px solid white;font:20px;Min-height:15px;border-radius:5px;}QDoubleSpinBox::up-button,QDoubleSpinBox::down-button,QSpinBox::up-button,QSpinBox::down-button{ width:0px;}QGroupBox{background-color:rgb(245,245,240);border: 2px solid white;border-radius:8px;margin-top:6px;}QGroupBox:title{color:rgb(24,24,58);subcontrol-origin: margin;left: 10px;}QGroupBox{font: 20px;}");

    //    Mat src = imread("C:\\Users\\jian.shen\\Desktop\\29.png");//("D:\\result.png");
    //    Mat dst;
    //    polar_to_cartesian_V1(src, dst);
    // Mat src, dst;
    // cartesian_to_polar(src,dst,src.rows);
    //polar_to_cartesian(src,dst,src.rows/2,src.rows*PI);
    //imwrite("D:\\aa90.png",dst);

    // showPixValue(src);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    ui->label_2->setText("");
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("文件对话框！"),
                                                    "F:",
                                                    tr("图片文件(*png *bmp *jpg);;"));
    if(fileName == "")
        return;
    m_path  = fileName;
    ui->pathEdit->setText(m_path);
    qDebug()<<fileName;
}

void MainWindow::on_pushButton_2_clicked()
{
    Mat img = imread(m_path.toStdString());
    if (!img.data)
    {
        ui->label_2->setText("Picture does not exist");
        return;
    }
    Mat img1, img2,dst,dst2;
    Point2f center = Point2f(img.cols / 2, img.rows/2); //极坐标在图像中的原点
    //正极坐标变换
    warpPolar(img, img1, Size(img.cols / 2,static_cast<int>(img.cols*3.1415926)), center,  static_cast<double>(center.x),
              INTER_LINEAR + WARP_POLAR_LINEAR );//
    //    flip(img1,img1,1);
    flip(img1,img1,0);
    //逆极坐标变换
    warpPolar(img1, img2, Size(img.rows,img.cols), center, static_cast<double>(center.x),
              INTER_LINEAR + WARP_POLAR_LINEAR + WARP_INVERSE_MAP);

    polar_to_cartesian(img,dst,img.rows/2,static_cast<int>(img.rows*PI)); //V2

    polar_to_cartesian_V1(img,dst2);

    imwrite("D:\\result.png",img1);
    imwrite("D:\\result_V1.png",dst);
    imwrite("D:\\result_V2.png",dst2);
    ui->label_2->setText("成功导出到D:\\result.png");
    waitKey(0);
}

void MainWindow::onTimeOut()
{

}

void MainWindow::polar2rectangle(int r, double angle)
{
    Mat src = imread("C:\\Users\\jian.shen\\Desktop\\28.bmp");
    int center_x = src.rows/2;
    Mat dst = Mat::ones(src.rows/2,int(src.cols*PI),CV_8UC1);
    for(int i = 1; i< r;i++)
    {
        for(int j = 1;j<angle;j++)
        {
            int x = i * cos(j*PI/180);
            int y = i * sin(j*PI/180);
            dst.ptr<uchar>(x)[y] = src.ptr<uchar>(center_x+x)[center_x+y];
        }
    }
    imwrite("D:\\bbb.png",dst);
}

void MainWindow::showPixValue(Mat &src)
{
    //    for(int x = 0 ;x < src.rows;x++)
    //    {
    //        for(int y = 0; y < src.cols; y++)
    //        {
    //            qDebug()<<"row,col"<<x<<","<<y<<"="<<src.ptr<uchar>(x)[y];
    //        }
    //    }

    //qDebug()<<src.ptr<uchar>(509)[256];
    Mat dst;
    cvtColor(src,dst,COLOR_BGR2GRAY);
    for(int i = 0;i<src.cols;i++)
    {
        qDebug()<<"("<<i<<",)"<<i<<")"<<src.ptr<uchar>(i)[220];
        //        for (int j = 0;j <src.rows; j++) {
        //        if(src.ptr<uchar>(i)[j] <40)
        //          qDebug()<<"("<<i<<",)"<<i<<")"<<src.ptr<uchar>(i)[274];
        //         }
    }

    ///--------------------------------------why this waring-------------------------------

}

double MainWindow::angle(QPoint center, int y, int x)
{
    int tempX = qAbs(center.x() - x);
    int tempY = qAbs(center.y() - y);
    double angle = qAtan2(tempY,tempX) *180 / PI;
    if(x >= center.x() && y <= center.y()) //第一象限
    {
        //        angle = angle;
        //        static int count_in1 = 0;
        //        qDebug()<< count_in1 ++<<"--->"<<angle;

    }
    else if(x <= center.x() && y <= center.y())
    {
        angle = (90 - angle) + 90;
    }
    else if(x <= center.x() && y >= center.y())
    {
        angle = angle + 180;
    }
    else if(x >= center.x() && y >= center.y())
    {
        angle = (90 - angle) + 270;
    }
    return angle;
}

///
/// \brief MainWindow::isInCricle
/// \param R 半径
/// \param y point y
/// \param x point x
/// \return true  = in the cricle
///
bool MainWindow::isInCricle(QPoint center, int R, int y, int x, double &r)
{
    int tempX = qAbs(center.x() - x);
    int tempY = qAbs(center.y() - y);
    r = qSqrt((tempX*tempX)+(tempY*tempY));
    if(r <= R)
        return true;
    return false;
}


bool MainWindow::cartesian_to_polar(cv::Mat& mat_c, cv::Mat& mat_p, int img_d)
{
    mat_p = cv::Mat::zeros(img_d, img_d, CV_8UC1);

    int line_len = mat_c.rows;
    int line_num = mat_c.cols;

    double delta_r = (2.0*line_len) / (img_d - 1); //半径因子
    double delta_t = 2.0 * PI / line_num; //角度因子

    double center_x = (img_d - 1) / 2.0;
    double center_y = (img_d - 1) / 2.0;

    for (int i = 0; i < img_d; i++)
    {
        for (int j = 0; j < img_d; j++)
        {
            double rx = j - center_x; //图像坐标转世界坐标
            double ry = center_y - i; //图像坐标转世界坐标

            double r = std::sqrt(rx*rx + ry*ry);

            if (r <= (img_d - 1) / 2.0)
            {
                double ri = r * delta_r;
                int rf = (int)std::floor(ri);
                int rc = (int)std::ceil(ri);

                if (rf < 0)
                {
                    rf = 0;
                }
                if (rc > (line_len - 1))
                {
                    rc = line_len - 1;
                }

                double t = std::atan2(ry, rx);

                if (t < 0)
                {
                    t = t + 2.0 * PI;
                }

                double ti = t / delta_t;
                int tf = (int)std::floor(ti);
                int tc = (int)std::ceil(ti);

                if (tf < 0)
                {
                    tf = 0;
                }
                if (tc > (line_num - 1))
                {
                    tc = line_num - 1;
                }

                mat_p.ptr<uchar>(i)[j] = interpolate_bilinear(mat_c, ri, rf, rc, ti, tf, tc);
            }
        }
    }

    return true;
}
uchar MainWindow::interpolate_bilinear(cv::Mat& mat_src, double ri, int rf, int rc, double ti, int tf, int tc)
{
    double inter_value = 0.0;

    if (rf == rc && tc == tf)
    {
        inter_value = mat_src.ptr<uchar>(rc)[tc];
    }
    else if (rf == rc)
    {
        inter_value = (ti - tf) * mat_src.ptr<uchar>(rf)[tc] + (tc - ti) * mat_src.ptr<uchar>(rf)[tf];
    }
    else if (tf == tc)
    {
        inter_value = (ri - rf) * mat_src.ptr<uchar>(rc)[tf] + (rc - ri) * mat_src.ptr<uchar>(rf)[tf];
    }
    else
    {
        double inter_r1 = (ti - tf) * mat_src.ptr<uchar>(rf)[tc] + (tc - ti) * mat_src.ptr<uchar>(rf)[tf];
        double inter_r2 = (ti - tf) * mat_src.ptr<uchar>(rc)[tc] + (tc - ti) * mat_src.ptr<uchar>(rc)[tf];

        inter_value = (ri - rf) * inter_r2 + (rc - ri) * inter_r1;
    }

    return (uchar)inter_value;
}

bool MainWindow::polar_to_cartesian(cv::Mat& mat_p, cv::Mat& mat_c, int rows_c, int cols_c)
{
    mat_c = cv::Mat::ones(rows_c, cols_c, CV_8UC1);
    cvtColor(mat_p,mat_p,COLOR_BGR2GRAY);

    int polar_d = mat_p.cols;
    double polar_r = polar_d / 2.0; // 圆图R

    double delta_r = polar_r / rows_c; //theta
    double delta_t = 2.0*PI / cols_c;  //bata

    double center_polar_x = (polar_d - 1) / 2.0;
    double center_polar_y = (polar_d - 1) / 2.0;

    for (int i = 0; i < cols_c; i++)
    {
        double theta_p = i * delta_t; //i列的角度（0-360）
        double sin_theta = std::sin(theta_p);
        double cos_theta = std::cos(theta_p);

        for (int j = 0; j < rows_c; j++)
        {
            double temp_r = j * delta_r; //j行在圆图上对应的半径

            int polar_x = (int)(center_polar_x + temp_r * cos_theta);
            int polar_y = (int)(center_polar_y - temp_r * sin_theta);
            if(polar_y != m_y || polar_x != m_x)//
            {
                mat_c.ptr<uchar>(j)[i] = mat_p.ptr<uchar>(polar_y)[polar_x];
            }
            else
                mat_c.ptr<uchar>(j)[i] = 255;
            m_x = polar_x;
            m_y = polar_y;
        }
    }
    flip(mat_c,mat_c,1);
    return true;
}

///
/// \brief MainWindow::polar_to_cartesian_V1
/// \param src 原图（圆）
/// \param dst 输出图像（方图)
/// \return
///
bool MainWindow::polar_to_cartesian_V1(Mat &src, Mat &dst)
{
    dst = cv::Mat::ones(src.rows/2, (static_cast<int>(src.cols * PI)), CV_8UC1);
    dst = 255;
    //cvtColor(src,src,COLOR_BGR2GRAY);

    int polar_d = src.cols;
    double width = polar_d * PI;
    QPoint center;
    double r = 0;
    center.setX(static_cast<int>((polar_d - 1) / 2.0));
    center.setY(static_cast<int>((polar_d - 1) / 2.0));
    for (int i = 0; i < src.cols; i++)
    {
        for (int j = 0; j < src.rows; j++)
        {
            if(src.ptr<uchar>(j)[i] < 50)
            {
                if(isInCricle(center,center.x(),j,i,r))
                {
                    double  angl = angle(center,j,i);

                    int x = static_cast<int>(width/360 * angl);
                    int y = static_cast<int>(r);
                    dst.ptr<uchar>(y)[x] = src.ptr<uchar>(j)[i];
                }
            }
        }
    }
    flip(dst,dst,1);
    return true;
}
