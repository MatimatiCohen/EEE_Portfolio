#include "IO_handle.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define MEM_SIZE 4096
#define MONITOR_SIZE 256
#define LINE_SIZE 6
#define NUM_OF_IRQ2 10
#define MAX_CLOCK_SIZE 9
#define LEDS_HEX 8
#define SECTOR_SIZE 128
#define PIXEL_SIZE 2
#define DISK_RW_TIME 1024


// opcodes

#define IN 19
#define OUT 20


#define IRQ0ENABLE 0
#define IRQ1ENABLE 1
#define IRQ2ENABLE 2
#define IRQ0STATUS 3
#define IRQ1STATUS 4
#define IRQ2STATUS 5
#define IRQHANDLER 6
#define IRQRETURN 7
#define CLKS 8
#define LEDS 9
#define DISPLAY7SEG 10
#define TIMERENABLE 11
#define TIMERCURRENT 12
#define TIMERMAX 13
#define DISKCMD 14
#define DISKSECTOR  15
#define DISKBUFFER  16
#define DISKSTATUS  17
#define RESERVED1  18
#define RESERVED2  19
#define MONITORADDR  20
#define MONITORDATA  21
#define MONITORCMD  22

void set_irq_value(GP* gp, int *io_registers) {
	gp->irq = (io_registers[IRQ0ENABLE] && io_registers[IRQ0STATUS]) ||
		(io_registers[IRQ1ENABLE] && io_registers[IRQ1STATUS]) ||
		(io_registers[IRQ2ENABLE] && io_registers[IRQ2STATUS]);
}

void allow_interupt_begin(GP* gp, int* io_registers) {
	io_registers[IRQRETURN] = gp->pc;
	gp->pc = io_registers[IRQHANDLER];
	gp->interupt_in_proccess = 1;
}




void init_frame_buffer(Monitor *moni) {
	int i , j ;
	for (i=0; i < MONITOR_SIZE; i++) {
		for (j=0; j < MONITOR_SIZE; j++) {
			moni->frame_buffer[i][j] = 0;
		}
	}
	moni->last_value_col = 0;
	moni->last_value_row = 0;
}

void set_frame_buffer(Monitor* moni, int* io_registers,GP *gp) {
	if (io_registers[MONITORCMD]) {
		int row_index = io_registers[MONITORADDR] / (MONITOR_SIZE);
		int col_index = io_registers[MONITORADDR] % (MONITOR_SIZE);
		moni->frame_buffer[row_index][col_index] = io_registers[MONITORDATA];
		moni->last_value_col = col_index;
		moni->last_value_row = row_index;
		io_registers[MONITORCMD] = 0;
	}
	else return;
}


	
void read_irq2in_data(irq2_manage* irq2, FILE* irq2in) {
	char line[MAX_CLOCK_SIZE];
	if (irq2->interrupts_left) {
		if (!irq2->current_irq2_pending) {
			if (fgets(line, MAX_CLOCK_SIZE, irq2in) != NULL) {
				irq2->irq2_current_clock = atoi(line);
				irq2->current_irq2_pending = true;
				return;
			}
			else {
				irq2->interrupts_left = false;
			}
		}
	}
	
}



void initialize_irq2_manage(irq2_manage* irq2) {
	irq2->irq2_current_clock = -2;
	irq2->interrupts_left = true;
	irq2->current_irq2_pending = false;

}

void set_irq2_status(irq2_manage* irq2, int *io_registers) {
	if (!irq2->interrupts_left) return;
	if (io_registers[CLKS]-1 >= irq2->irq2_current_clock) { ///clk>=irq_timing_arr[interrupt occured]
		io_registers[IRQ2STATUS] = 1;
		irq2->current_irq2_pending = false;
	}
}

void timer_update(int* io_registers) {
	if (io_registers[TIMERENABLE]) {
		io_registers[TIMERCURRENT]++;

		if (io_registers[TIMERCURRENT] == io_registers[TIMERMAX]) {
			io_registers[TIMERCURRENT] = 0;
			io_registers[IRQ0STATUS] = 1;
		}
	}
}


void initialize_hard_disk(hard_disk *hard_disk, char** argv) {
	FILE* diskin = NULL;
	fopen_s(&diskin, argv[2], "r");
	assert(diskin != NULL);
	char line[LINE_SIZE];
	for (int row = 0; row < SECTOR_SIZE; row++) {
		for (int col = 0; col < SECTOR_SIZE; col++) {
			if (fgets(line, LINE_SIZE, diskin) == NULL) break; 
			hard_disk->memmory[row][col] = (int)strtol(line,NULL,16);	
			getc(diskin);
		}
	}
	hard_disk->start_clock = 0;
	if (diskin) {
		fclose(diskin);
	}

}

void hard_disk_operation(hard_disk* hd, int* io_registers, char main_memmory[][LINE_SIZE]) {
	int i;
	int address=0;
	char disk_word[LINE_SIZE];
	rsize_t buffer = LINE_SIZE;
	if (!io_registers[DISKSTATUS] && io_registers[DISKCMD]) {
		io_registers[DISKSTATUS] = 1;
		hd->start_clock = io_registers[CLKS];
	}
	if (io_registers[DISKSTATUS] && (io_registers[CLKS] - hd->start_clock >= DISK_RW_TIME)) {
		if (io_registers[DISKCMD] == 1) { //read sector
			address = io_registers[DISKBUFFER];
			for (i = 0; i < SECTOR_SIZE; i++) {
				dec_to_n_chars_hex(disk_word, hd->memmory[io_registers[DISKSECTOR]][i],LINE_SIZE-1);
				strcpy_s(main_memmory[address],buffer, disk_word);
				address++;
			}
		}
		if (io_registers[DISKCMD] == 2) { //write sector
			address = io_registers[DISKBUFFER];
			for (i = 0; i < SECTOR_SIZE; i++) {
				hd->memmory[io_registers[DISKSECTOR]][i] = (int)strtol(main_memmory[address], NULL, 16);
				address++;
			}
		}
		io_registers[DISKSTATUS] = 0;
		io_registers[DISKCMD] = 0;
		io_registers[IRQ1STATUS] = 1;
	}
}



