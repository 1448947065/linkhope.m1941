#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "worker.h"
#include <QMainWindow>
#include <worker.h>
#include <QThread>
#include <QComboBox>
#include <QTimer>
#include <QDateTime>
#include <QMutex>
extern int status12;
extern int flagStringPtr;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;
             }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow *ui;

    ~MainWindow();
    void deviceInit();
    void deviceDeinit();
    void on_textEdit_destroyed();
    void qtimeSlot();
    void start_time();
    int on_comboBox_2_activated();
    QString *read_get_flag();
signals:
    void sigStartCapture(void *ptr, int fpga_fd);
    void sigStopCapture();

private slots:

    void doUpdateData(void *buffer);
    void on_btn_start_clicked();
    void on_label_linkActivated();
    void on_comboBox_activated();

    void on_label_4_linkActivated();

    void on_label_5_linkActivated();
    void label();
    void on_radioButton_clicked();
    void Button();
    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_label_2_linkActivated();

    void on_pushButton_clicked();

    void on_clearButton_clicked();

    void on_pushButton_2_clicked();

    void on_textBrowser_destroyed();

    //void on_textEdit_destroyed();


    void on_textBrowser_2_destroyed();

    //void on_textBrowser_3_anchorClicked(const QUrl &arg1);

    void on_textBrowser_3_destroyed();

    void on_textBrowser_4_destroyed();

   // void on_textBrowser_anchorClicked(const QUrl &arg1);

    //void on_comboBox_2_activated(const QString &arg1);





    void on_comboBox_2_activated(const QString &arg1);

private:
    Worker *worker;
    QThread *workerThread;
    bool startFlag;
    void *ptr;
    int fpga_fd;
    QTimer *timer;
    //int flag2;
    QMutex flagMutex;

};
#endif // MAINWINDOW_H
