#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QBitmap>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    union char_k{
        struct {
            uchar
            bit0,
            bit1,
            bit2,
            bit3,
            bit4,
            bit5,
            bit6,
            bit7;
        };
        uchar v;
    };
};

#endif // MAINWINDOW_H
