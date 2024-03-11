#ifndef IO_HANDLE_H
#define IO_HANDLE_H
#include "command_handle.h"
#include <stdio.h>

#define MONITOR_SIZE 256
#define SECTOR_SIZE 128


typedef struct Monitor {
	int frame_buffer[MONITOR_SIZE][MONITOR_SIZE];
	int last_value_col;
	int last_value_row;
}Monitor;


typedef struct Irq2_Manage {
	int irq2_current_clock;
	bool interrupts_left;
	bool current_irq2_pending;
}irq2_manage;


typedef struct Hard_Disk {
	int memmory[SECTOR_SIZE][SECTOR_SIZE];
	int start_clock;
}hard_disk;


void set_irq_value(GP* gp, int *io_registers);
void set_irq2_status(irq2_manage *irq2, int *io_registers );
void read_irq2in_data(irq2_manage *irq2, FILE* irq2in);
void initialize_irq2_manage(irq2_manage *irq2);
void allow_interupt_begin(GP* gp, int* io_registers);
void init_frame_buffer(Monitor *moni);
void set_frame_buffer(Monitor* moni, int* io_registers, GP* gp);
void timer_update(int *io_registers);
void initialize_hard_disk(hard_disk *hd, char** argv);
void hard_disk_operation(hard_disk* hd, int* io_registers, char main_memmory[][LINE_SIZE]);




#endif