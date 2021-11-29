#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <mcclienthelper.h>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString m_ipAddress;
    uint16_t m_port;
    Ui::MainWindow *ui;
    QTimer *m_pTimer;
    McClientHelper *m_pMcHelper;
     bool m_connectStatus;
public:
    void onPlcTimerOut();
};
#endif // MAINWINDOW_H
