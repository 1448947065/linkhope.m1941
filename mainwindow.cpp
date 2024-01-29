 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "utilities/lhutility.h"
#include "common.h"
#include <QDebug>
#include <sys/time.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <QTimer>
#include <QTime>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <QDateTime>
#include "worker.h"
#include <QMutex>
int state = 0;
int status12 = 0;
int flag = 0;
int fpga_fd;
int flagStringPtr;
QString strTime;
QTimer *timer1;
void *ptr;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    startFlag = false;
    fpga_fd = -1;
    ptr = NULL;
    ui->setupUi(this);
    workerThread = new QThread;
    worker = new Worker;

    worker->moveToThread(workerThread);
    timer = new QTimer(this);
    timer1 = new QTimer(this);
    connect(workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(sigStartCapture(void*, int)), worker, SLOT(doCapture(void*, int)));
    connect(worker, SIGNAL(sigSendData(void*)), this, SLOT(doUpdateData(void*)));
    connect(timer,&QTimer::timeout,this,&MainWindow::start_time);
    connect(timer1,&QTimer::timeout,this,&MainWindow::qtimeSlot);
    timer1->setInterval(50);
    timer1->start();
    on_textBrowser_2_destroyed();
    on_label_linkActivated();
    workerThread->start();
    on_comboBox_activated();
    //on_pushButton_2_clicked();
    on_textBrowser_destroyed();
    //on_label_3_linkActivated();
    label();
    Button();
    deviceInit();
}

MainWindow::~MainWindow()
{
    worker->resetContFlag();

    usleep(3000000);
    deviceDeinit();
    if (workerThread) {
        workerThread->exit();
        workerThread->wait();
    }
    delete timer;
    delete timer1;
    delete ui;

}

void MainWindow::deviceInit()
{
    qDebug() << "deviceInit";
    char *device = "/dev/xdma0_c2h_0";
    char *buf;
    ptr = fpga_reg_init();
    qDebug() << ptr;
    if (ptr == NULL) {
        qDebug() << "fpga_reg_init failed";
        return;
    }

    fpga_fd = open(device, O_RDWR);
    if (fpga_fd < 0) {
        qDebug() << "unable to open device";
        fpga_reg_deinit(ptr);
        return;
    }

}

void MainWindow::deviceDeinit()
{
    qDebug() << "deviceDeinit";
    fpga_reg_deinit(ptr);
}
void sigStartCapture(void *ptr, int fpga_fd);
void MainWindow::start_time()
{
    void *ptr = fpga_reg_init();
    int flag = fpga_reg_get(ptr,0x100);
    if(flag)
     {

        emit sigStartCapture(ptr, fpga_fd);
        //on_comboBox_2_activated();
    }else {

        }


}
void MainWindow::doUpdateData(void *buffer)
{
    uint8_t *buf = (uint8_t*)buffer;
    uint64_t length;
    QString s = NULL;
    QString time = strTime;
    //ui->pte_data->setPlainText("strTime");
    on_textBrowser_3_destroyed();
    on_textBrowser_4_destroyed();
   // on_comboBox_2_activated();
    length = fpga_reg_get(ptr,0x108)+2;
    for (int i = 2; i < length; i++) {
    s += QString("%1").arg(buf[i], 0, 16) + " "+" ";
    }
    qDebug() << s;
    if(s!=NULL)flag++;
    on_textBrowser_destroyed();

    ui->pte_data->appendPlainText(time+":\n"+s);
}

void MainWindow::on_btn_start_clicked()
{
    emit sigStartCapture(ptr, fpga_fd);
    //qDebug() << "Button clicked!";
}


void MainWindow::on_label_linkActivated()
{
    ui->label->setText(QStringLiteral("（1）协议类型"));
}

void MainWindow::on_comboBox_activated()
{

}

void MainWindow::on_label_4_linkActivated()
{
    ui->label_4->setText(QStringLiteral("（2）本地IP地址"));
}

void MainWindow::on_label_5_linkActivated()
{
    ui->label_5->setText(QStringLiteral("（3）本地端口号"));
}
void MainWindow::label()
{
    on_label_4_linkActivated();
    on_label_5_linkActivated();
    on_label_2_linkActivated();
}

void MainWindow::on_radioButton_clicked()
{
    ui->checkBox->setText(QStringLiteral("自动换行"));
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->checkBox_2->setText(QStringLiteral("循环接收数据"));
}

void MainWindow::on_radioButton_3_clicked()
{
    ui->checkBox_3->setText(QStringLiteral("接收时间显示"));
}
void MainWindow::Button()
{

    on_radioButton_clicked();
    on_radioButton_2_clicked();
    on_radioButton_3_clicked();
}

void MainWindow::on_label_2_linkActivated()
{
    ui->label_2->setText(QStringLiteral("设置循环时间间隔:"));
}

void MainWindow::on_pushButton_clicked()
{
    state = 1-state;
    if(state == 0){
        status12 = 2;
        ui->pushButton->setText(QStringLiteral("连接"));
        timer->stop();

        }
    else{
        on_comboBox_2_activated();
        //read_get_flag();
        status12 = 1;
        ui->pushButton->setText(QStringLiteral("断开连接"));
        QString text = ui->comboBox_4->currentText();
        bool ok=true;
        int value = text.toInt(&ok,10);
        timer->setInterval(value);
        timer->start();
    }
}

void MainWindow::on_clearButton_clicked()
{
    ui->pte_data->clear();
}

void MainWindow::on_pushButton_2_clicked()
{

    //on_textBrowser_destroyed();
    flag = 0;
    //on_textBrowser_destroyed();
    on_textBrowser_destroyed();
}

void MainWindow::on_textBrowser_destroyed()
{
   QString a = QString::number(flag);
    ui->textBrowser->setText(a);
    //ui->textBrowser_2->setText(a);
}


void MainWindow::on_textEdit_destroyed()
{

}
void MainWindow::qtimeSlot()
{
   QTime qtimeObj = QTime::currentTime();
   strTime = qtimeObj.toString("hh:mm:ss ap");
   strTime.prepend(" Time is: ");
   ui->textBrowser_2->setText(strTime);

}

void MainWindow::on_textBrowser_2_destroyed()
{

}

void MainWindow::on_textBrowser_3_destroyed()
{
    void *ptr = fpga_reg_init();
    QString addr;
    switch(flagStringPtr)
    {
        case 1:
             addr= QString::number(fpga_reg_get(ptr,0x104),16);break;
        case 2:
             addr = QString::number(fpga_reg_get(ptr,0x10c),16);break;
        case 3:
             addr = QString::number(fpga_reg_get(ptr,0x114),16);break;
        case 4:
             addr = QString::number(fpga_reg_get(ptr,0x11c),16);break;

    }

    ui->textBrowser_3->setText(addr);
}

void MainWindow::on_textBrowser_4_destroyed()
{
    void *ptr = fpga_reg_init();
    QString length;
    switch(flagStringPtr)
    {
        case 1:
             length= QString::number(fpga_reg_get(ptr,0x108),16);break;
        case 2:
             length = QString::number(fpga_reg_get(ptr,0x110),16);break;
        case 3:
             length = QString::number(fpga_reg_get(ptr,0x118),16);break;
        case 4:
             length = QString::number(fpga_reg_get(ptr,0x120),16);break;

    }
    ui->textBrowser_4->setText(length);
}


int MainWindow::on_comboBox_2_activated()
{
    QString text = ui->comboBox_2->currentText();
    qDebug() << "text is" << text;
    bool ok=true;
    int flag2 = text.toInt(&ok,10);
    flagStringPtr = flag2;
    qDebug() << "mainflag is" << flagStringPtr;
    return 0;
}
