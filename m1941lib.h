#ifndef M1941LIB_H
#define M1941LIB_H

#ifdef IMPORT
extern "C" {
#endif

uint64_t fpga_get_buffer_addr(void *ptr,uint8_t status);
uint64_t fpga_get_buffer_length(void *ptr,uint8_t status);
uint64_t fpga_get_ready_flag(void *ptr);
void fpga_initiate_data_read(void *ptr,uint8_t status);
void* fpga_reg_init();
void fpga_reg_deinit(void *ptr);
void fpga_reg_set(void* ptr, unsigned int offset, unsigned int value);
unsigned int fpga_reg_get(void* ptr, unsigned int offset);
ssize_t read_to_buffer(int fd, char *buffer, uint64_t size, uint64_t base);
#ifdef IMPORT
}
#endif
#endif // M1941LIB_H
