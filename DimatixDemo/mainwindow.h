#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <windows.h>
#include "HS.Print.Core.h"
#include <QFileDialog>
#include <QTimer>
#include <QSharedPointer>
#include <QImage>
#include <QEventLoop>
#include "mcclient.h"
#include "mcclienthelper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


#if defined(WIN32) ||  defined(_WIN32)
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    size_t MBCS2WCS(const char * src, wchar_t* wstrResult, size_t sizeInWords)
    {
        if (src == nullptr)
            return 0;

    #if defined(WIN32) ||  defined(_WIN32)

        sizeInWords = ::MultiByteToWideChar(CP_ACP, 0, src, -1, wstrResult, (int)(wstrResult == nullptr ? 0 : sizeInWords));

    #else
        char *  strLocale = setlocale(LC_ALL, "");
        //开始转换
        mbstowcs_s(&sizeInWords, wstrResult, wstrResult == nullptr ? 0 : sizeInWords, src, _TRUNCATE);
        setlocale(LC_ALL, strLocale);
    #endif

        return sizeInWords;
    }

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_startbtn_clicked();

    void on_endBtn_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();
    void onTimeout();

    void on_noTypecomboBox_currentIndexChanged(int index);

    void saveSplitImage(PrintInfo* swatheInfo);

    void on_x_add_pressed();

    void on_x_dcc_released();

    void on_x_add_released();

    void on_x_dcc_pressed();

    void on_x_start_clicked();

    void on_x_end_clicked();

private:
    Ui::MainWindow *ui;
     SwatheSplitCfg *m_pSplitCfg;
    McClientHelper *m_pMcClientHelper;//McClient
    bool m_mcIsConnect = false;
    bool m_waveIsSet  = false;
    // set print cfg
    PrintCfg m_cfg;

    QString m_path;
    PrintInfo *m_print;
    QImage *m_dst1;
    bool m_canStart = false;

    PHType m_type;
    QTimer *m_timer;
    QTimer *m_pMcTimer;
    int m_NolStatus = 0;
    int  GenSwatheInfoFromFile_korol(const char * file, const SwatheSplitCfg * cfg, PrintInfo ** printInfo);
    void testGenSwatheInfoFromFile_korolFunc(const SwatheSplitCfg *cfg, PrintInfo **printInfo);

    void onMcTimerTimout();
    void sleep(int time);

    //------PLC-------
    bool move_x(int port);
    bool move_u(int port);
    bool move_y(int port);

protected:
    //void WindowsColseEvent(QEvent *event);


};
#endif // MAINWINDOW_H
