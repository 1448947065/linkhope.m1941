#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "m1941lib.h"

#include "xdma/cdev_sgdma.h"

#define RW_MAX_SIZE	0x7ffff000

static int eop_flush = 0;

int executeShellCommand(const char *command, char *output, int max_output_size) {
    FILE *fp;
    char buffer[256];
    int total_output_length = 0;

    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        int length = strlen(buffer);
        if (total_output_length + length >= max_output_size) {
            // Output buffer overflow
            pclose(fp);
            return -1;
        }
        strcat(output, buffer);
        total_output_length += length;
    }

    int status = pclose(fp);
    if (status == -1) {
        perror("pclose");
        return -1;
    }

    return status;
}

void* fpga_reg_init()
{
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1) {
            printf("Cannot open /dev/mem\n");
            return NULL;
    }
    char output[256] = "";
    const char *command = "lspci -s $(lspci -n | grep \"10ee:7038\" | awk '{print $1}') -vvv | grep \"Region 0\" | awk '{print $5}'"; 
    uint64_t fpga_addr;
    int status = executeShellCommand(command, output, sizeof(output));

    if (status != 0) {
        printf("Cannot find FPGA device, ret=%d\n", status);
	close(mem_fd);
	return NULL;
    }

    fpga_addr = strtol(output, NULL, 16);

    if (fpga_addr == 0) {
        printf("Internal error while parsing the BAR0 value\n");
	close(mem_fd);
	return NULL;
    } else {
	//printf("BAR0 addr = 0x%lx\n", fpga_addr);
    }
    
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, fpga_addr);
    if (ptr == MAP_FAILED) {
            close(mem_fd);
            printf("Cannot map memory\n");
            return NULL;
    }
    close(mem_fd);
    return ptr;
}
/* 读数据初始化，产生初始化脉冲 */
void fpga_initiate_data_read(void *ptr,uint8_t status)
{
    uint64_t state = 0;
    for(int i=0;i<4;i++)
    {
        if(status & 1<<i)
        {
            state = 1 << i;
            fpga_reg_set(ptr, 0x0, state);
            state = state &(~(1 << i));
            fpga_reg_set(ptr, 0x0, state);
            usleep(1);

        }
    }
    
}
/* 读地址 */
uint64_t fpga_get_buffer_addr(void *ptr,uint8_t status)
{
    uint64_t buffer_addr;
    switch (status)
    {
        case 1:buffer_addr = fpga_reg_get(ptr, 0x104);break;
        case 2:buffer_addr = fpga_reg_get(ptr, 0x10c);break;
        case 3:buffer_addr = fpga_reg_get(ptr, 0x114);break;
        case 4:buffer_addr = fpga_reg_get(ptr, 0x11c);break;
        default:break;
    }
    return buffer_addr;
}
/* 读长度 */
uint64_t fpga_get_buffer_length(void *ptr,uint8_t status)
{
    uint64_t length = 0;
    switch (status)
    {
        case 1:length = fpga_reg_get(ptr, 0x108);break;
        case 2:length = fpga_reg_get(ptr, 0x110);break;
        case 3:length = fpga_reg_get(ptr, 0x118);break;
        case 4:length = fpga_reg_get(ptr, 0x120);break;
        default:break;
    }
    return length+2;
}
/* 读标志位,哪位有数哪位为1 */
uint64_t fpga_get_ready_flag(void *ptr)
{
    uint64_t flag = fpga_reg_get(ptr, 0x100);
    return flag;
}

void fpga_reg_deinit(void *ptr)
{
    if (ptr) {
        munmap(ptr, 4096);
    }
    // if (mem_fd) {
    //     close(mem_fd);
    // }
}

void fpga_reg_set(void* ptr, unsigned int offset, unsigned int value)
{
    *(volatile unsigned int*)(ptr+offset) = value;
}

unsigned int fpga_reg_get(void* ptr, unsigned int offset)
{
    return *(volatile unsigned int*)(ptr+offset);
}

ssize_t read_to_buffer(int fd, char *buffer, uint64_t size,
			uint64_t base)
{
	ssize_t rc;
	uint64_t count = 0;
	char *buf = buffer;
	off_t offset = base;
	int loop = 0;

	while (count < size) {
		uint64_t bytes = size - count;

		if (bytes > RW_MAX_SIZE)
			bytes = RW_MAX_SIZE;

                if (offset) {
			rc = lseek(fd, offset, SEEK_SET);
			if (rc != offset) {

				perror("seek file");
				return -EIO;
			}
               }

		//read data from file into memory buffer
		rc = read(fd, buf, bytes);
		//printf("read %ld bytes @ offest 0x%lx.\n",rc, offset);
		if (rc < 0) {
//			fprintf(stderr, "%s, read 0x%lx @ 0x%lx failed %ld.\n",
//				fname, bytes, offset, rc);
			perror("read file");
			return -EIO;
		}

		count += rc;
		if (rc != bytes) {
//			fprintf(stderr, "%s, read underflow 0x%lx/0x%lx @ 0x%lx.\n",
//				fname, rc, bytes, offset);
			break;
		}

		buf += bytes;
		offset += bytes;
		loop++;
	}

	if (count != size && loop)
//		fprintf(stderr, "%s, read underflow 0x%lx/0x%lx.\n",
//			fname, count, size);
	return count;
}

