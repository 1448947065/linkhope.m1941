#include "worker.h"
#include "common.h"
#include <QDebug>
#include <sys/time.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <QTimer>
#include <time.h>
#include "ui_mainwindow.h"
#include <sys/ioctl.h>
#include "xdma/cdev_sgdma.h"
int flag1 = 2;

int read_data(int fpga_fd, uint64_t addr, uint64_t aperture,
            uint64_t size, uint64_t offset, uint64_t count,
            char *allocated);

Worker::Worker(QObject *parent) : QObject(parent)
{
    contFlag = true;
    exitFlag = false;
    fpga_fd = -1;
    ptr = NULL;

}

Worker::~Worker()
{

}

void Worker::setContFlag()
{
    contFlag = true;
}

void Worker::resetContFlag()
{
    contFlag = false;
}
char *allocated = NULL;
void Worker::doCapture(void *ptr, int fpga_fd)
{

    qDebug() << "doCapture";
    //void *ptr = NULL;
    uint64_t buffer, length, preset_length = 4096, ready_read_flag = 0;
    int rc;
    if (allocated != NULL) {
            free(allocated);
            allocated = NULL;
        }
    rc = posix_memalign((void **)&allocated, 4096, preset_length + 4096);

    if (rc && !allocated) {
        qDebug() << "allocate memory failed";
        return;
    }

    memset(allocated, 0, preset_length);

    usleep(10000);

    int frame_count = 0;

    flag1 = 0;
    while (contFlag) {
        // 1. test flag
        do {
            ready_read_flag = fpga_get_ready_flag(ptr);
            if((ready_read_flag == 0x4)||(ready_read_flag == 0x1)||(ready_read_flag == 0x8)||(ready_read_flag == 0x8))
                qDebug() << "ready_read_flag:" << ready_read_flag;
        } while ((ready_read_flag & 0x4 == 0)||(ready_read_flag & 0x1 == 0)||(ready_read_flag & 0x8 == 0)||(ready_read_flag & 0x2 == 0));
            extern int flagStringPtr;
            extern int status12;
           if(status12 == 1)
            if (ready_read_flag & 0x4||ready_read_flag & 0x1 ||ready_read_flag & 0x8||ready_read_flag & 0x2 ) {
            // 2. read data from buffer0
            flag1 = flagStringPtr;
            qDebug() << "flag1 is "<<flag1;
            qDebug() << "frame_count is "<<frame_count;
            qDebug() << "status12 is "<<status12;
            switch(flag1)
            {
                case 1:
                    {
                        fpga_initiate_data_read(ptr,0x1);
                        buffer = fpga_get_buffer_addr(ptr,flag1);
                        length = fpga_get_buffer_length(ptr,flag1)+2;
                        if(ready_read_flag & 0x1)
                        {
                            qDebug() << "a";
                            qDebug() << "=======";
                            qDebug() << "ADDR" << hex << buffer;
                            qDebug() << "LEN" << hex << length;
                            qDebug() << "=======";
                            read_data(fpga_fd, buffer, 0, length, 0, 1, allocated);
                            emit sigSendData(allocated);
                            qDebug() << "allocated"<<allocated;
                        }
                    }break;
                case 2:
                    {

                        fpga_initiate_data_read(ptr,0x2);
                        buffer = fpga_get_buffer_addr(ptr,flag1);
                        length = fpga_get_buffer_length(ptr,flag1)+2;
                        qDebug() << ready_read_flag;
                        if(ready_read_flag & 0x2)
                        {
                            qDebug() << "b";
                            qDebug() << "=======";
                            qDebug() << "ADDR" << hex << buffer;
                            qDebug() << "LEN" << hex << length;
                            qDebug() << "=======";
                            read_data(fpga_fd, buffer, 0, length, 0, 1, allocated);
                            //allocated = "adc";
                            emit sigSendData(allocated);

                            qDebug() << allocated;
                        }
                    }break;
                case 3:
                    {
                        fpga_initiate_data_read(ptr,0x4);
                        buffer = fpga_get_buffer_addr(ptr,flag1);
                        length = fpga_get_buffer_length(ptr,flag1)+2;
                        if(ready_read_flag & 0x4)
                        {
                            qDebug() << "c";
                            qDebug() << "=======";
                            qDebug() << "ADDR" << hex << buffer;
                            qDebug() << "LEN" << hex << length;
                            qDebug() << "=======";
                            read_data(fpga_fd, buffer, 0, length, 0, 1, allocated);
                            emit sigSendData(allocated);
                        }
                    }break;
                case 4:
                    {
                        fpga_initiate_data_read(ptr,0x8);
                        buffer = fpga_get_buffer_addr(ptr,flag1);
                        length = fpga_get_buffer_length(ptr,flag1)+2;
                        if(ready_read_flag & 0x8)
                        {
                            qDebug() << "d";
                            qDebug() << "=======";
                            qDebug() << "ADDR" << hex << buffer;
                            qDebug() << "LEN" << hex << length;
                            qDebug() << "=======";
                            read_data(fpga_fd, buffer, 0, length, 0, 1, allocated);
                            emit sigSendData(allocated);
                        }
                    }break;

            }
            //goto out;
        }

    }
//out:
   // free(allocated);
}

int read_data(int fpga_fd, uint64_t addr, uint64_t aperture,
            uint64_t size, uint64_t offset, uint64_t count,
            char *allocated)
{
    ssize_t rc = 0;
    size_t bytes_done = 0;
    uint64_t i;
    char *buffer = NULL;
//    int fpga_fd;
    int underflow = 0;
//    int eop_flush = 0;

//    if (eop_flush)
//        fpga_fd = open(devname, O_RDWR | O_TRUNC);
//    else
//        fpga_fd = open(devname, O_RDWR);

//    if (fpga_fd < 0) {
//        return -EINVAL;
//    }

    buffer = allocated + offset;

    for (i = 0; i < count; i++) {
        if (aperture) {
            struct xdma_aperture_ioctl io;

            io.buffer = (unsigned long)buffer;
            io.len = size;
            io.ep_addr = addr;
            io.aperture = aperture;
            io.done = 0UL;

            rc = ioctl(fpga_fd, IOCTL_XDMA_APERTURE_R, &io);
            if (rc < 0 || io.error) {
                goto out;
            }

            bytes_done = io.done;
        } else {
            rc = read_to_buffer(fpga_fd, buffer, size, addr);
            //qDebug() << "rc is:";                                                                                    is:";
            qDebug() << "rc is:" << rc;
            //rc = -1;
            if (rc < 0)
                //goto out;
            bytes_done = rc;
        }

        if (bytes_done < size) {
            underflow = 1;
        }
    }

    if (!underflow) {
        rc = 0;
    } else
        rc = -EIO;

out:
    //close(fpga_fd);
    return rc;
}


