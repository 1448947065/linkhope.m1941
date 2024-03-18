#ifndef WORKER_H
#define WORKER_H
#include "ui_mainwindow.h"
#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QObject *parent = 0);
    ~Worker();

    void setContFlag();
    void resetContFlag();
    void setExitFlag();
    void resetExitFlag();
    char* readContFlag();
    //QString static on_comboBox_2_activated();
signals:
    void sigSendData(void *buffer);
private slots:
    void doCapture(void *ptr, int fpga_fd);

private:
    bool contFlag;
    bool exitFlag;
    int fpga_fd;
    void *ptr;

};

#endif // LHIPMIWORKER_H

