#include "files_printer.h"


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


void print_monitor(Monitor* moni, char** argv) {
	FILE* monitor_txt_file = NULL, * monitor_yuv_file = NULL;
	unsigned char hex_num[PIXEL_SIZE + 1];
	int last_row = (MONITOR_SIZE)*moni->last_value_col + moni->last_value_row;
	fopen_s(&monitor_txt_file, argv[12], "w");
	fopen_s(&monitor_yuv_file, argv[13], "wb");
	int i, j;
	for (i = 0; i < MONITOR_SIZE; i++) {
		for (j = 0; j < MONITOR_SIZE; j++) {
			if ((i * MONITOR_SIZE + j) <= last_row) {
				dec_to_n_chars_hex(hex_num, moni->frame_buffer[i][j], 2);
				if (monitor_txt_file) {
					fprintf_s(monitor_txt_file, "%s\n", hex_num);
				}
			}
		}
	}
	for (i = 0; i < MONITOR_SIZE; i++) {
		for (j = 0; j < MONITOR_SIZE; j++) {
			if (monitor_yuv_file) {
				unsigned char pix = moni->frame_buffer[i][j];
				fwrite(&pix, 1, 1, monitor_yuv_file);
			}
		}
	}


	if (monitor_txt_file) {
		fclose(monitor_txt_file);
	}
	if (monitor_yuv_file) {
		fclose(monitor_yuv_file);
	}
}


void print_cycles_file(int* io_registers, char** argv) {
	FILE* cycles_file = NULL;
	fopen_s(&cycles_file, argv[8], "w");
	if (cycles_file) {
		fprintf_s(cycles_file, "%d\n", io_registers[CLKS]);
		fclose(cycles_file);
	}
}



void print_main_memmory(char main_memmory[][LINE_SIZE], char** argv) {
	int i;
	int last_non_zero_val = MEM_SIZE - 2;
	FILE* memout_file = NULL;
	while (!strcmp(main_memmory[last_non_zero_val], "00000")) {
		last_non_zero_val--;
	}
	fopen_s(&memout_file, argv[4], "w");
	for (i = 0; i <= last_non_zero_val; i++) {
		if (memout_file) {
			fprintf(memout_file, "%s\n", main_memmory[i]);
		}
	}
	if (memout_file) {
		fclose(memout_file);
	}
}


void print_regout_file(int* registers, char** argv) {
	int i;
	char hex_num[LINE_SIZE];
	FILE* regout_file = NULL;
	fopen_s(&regout_file, argv[5], "w");
	for (i = 2; i <= 15; i++) {
		if (regout_file) {
			fprintf(regout_file, "000");
			dec_to_n_chars_hex(hex_num, registers[i], LINE_SIZE - 1);
			fprintf(regout_file, "%s\n", hex_num);
		}
	}
	if (regout_file) {
		fclose(regout_file);
	}
}


void print_trace_file(Command* cmd, int* registers, GP* gp, FILE* trace_file) {
	int i;
	char reg[LINE_SIZE];
	char pc[4];
	char opcode[3];
	char rd[2];
	char rs[2];
	char rt[2];
	dec_to_n_chars_hex(pc, gp->pc, 3);
	dec_to_n_chars_hex(opcode, cmd->opcode, 2);
	dec_to_n_chars_hex(rd, cmd->rd, 1);
	dec_to_n_chars_hex(rs, cmd->rs, 1);
	dec_to_n_chars_hex(rt, cmd->rt, 1);
	if (trace_file) {
		fprintf(trace_file, "%s ", pc);
		fprintf(trace_file, "%s", opcode);
		fprintf(trace_file, "%s", rd);
		fprintf(trace_file, "%s", rs);
		fprintf(trace_file, "%s ", rt);
		for (i = 0; i <= 15; i++) {
			dec_to_n_chars_hex(reg, registers[i], LINE_SIZE - 1);
			if (reg[i] == 'F') {
				fprintf(trace_file, "FFF"); 
			}
			else {
				fprintf(trace_file, "000");
			}
			fprintf(trace_file, "%s", reg);
			if (i < 15) {
				fprintf(trace_file, " ");
			}
		}
		fprintf(trace_file, "\n");
	}
}


void print_hwregrace_file(Command* cmd, int* registers, int* io_registers, FILE* hwregtrace_file) {
	char io_reg[LINE_SIZE];
	int io_reg_num = registers[cmd->rs] + registers[cmd->rt];
	fprintf(hwregtrace_file, "%d ", io_registers[CLKS] - 1);
	if (cmd->opcode == IN) {
		fprintf(hwregtrace_file, "READ ");
	}
	else if (cmd->opcode == OUT) {
		fprintf(hwregtrace_file, "WRITE ");
	}
	switch (io_reg_num)
	{
	case IRQ0ENABLE: fprintf(hwregtrace_file, "irq0enable ");
		break;
	case IRQ1ENABLE: fprintf(hwregtrace_file, "irq1enable ");
		break;
	case IRQ2ENABLE: fprintf(hwregtrace_file, "irq2enable ");
		break;
	case IRQ0STATUS: fprintf(hwregtrace_file, "irq0status ");
		break;
	case IRQ1STATUS: fprintf(hwregtrace_file, "irq1status ");
		break;
	case IRQ2STATUS: fprintf(hwregtrace_file, "irq2status ");
		break;
	case IRQHANDLER: fprintf(hwregtrace_file, "irqhandler ");
		break;
	case IRQRETURN: fprintf(hwregtrace_file, "irqreturn ");
		break;
	case LEDS: fprintf(hwregtrace_file, "leds ");
		break;
	case DISPLAY7SEG: fprintf(hwregtrace_file, "display7seg ");
		break;
	case TIMERENABLE: fprintf(hwregtrace_file, "timerenable ");
		break;
	case TIMERCURRENT: fprintf(hwregtrace_file, "timercurrent ");
		break;
	case TIMERMAX: fprintf(hwregtrace_file, "timermax ");
		break;
	case DISKCMD: fprintf(hwregtrace_file, "diskcmd ");
		break;
	case DISKSECTOR: fprintf(hwregtrace_file, "disksector ");
		break;
	case DISKBUFFER: fprintf(hwregtrace_file, "diskbuffer ");
		break;
	case DISKSTATUS: fprintf(hwregtrace_file, "diskstatus ");
		break;
	case RESERVED1: fprintf(hwregtrace_file, "reserved ");
		break;
	case RESERVED2: fprintf(hwregtrace_file, "reserved ");
		break;
	case MONITORADDR: fprintf(hwregtrace_file, "monitoraddr ");
		break;
	case MONITORDATA: fprintf(hwregtrace_file, "monitordata ");
		break;
	case MONITORCMD: fprintf(hwregtrace_file, "monitorcmd ");
		break;
	default:
		break;
	}
	dec_to_n_chars_hex(io_reg, io_registers[io_reg_num], LINE_SIZE - 1);
	if (io_reg[0] == 'F') {
		fprintf(hwregtrace_file, "FFF"); 
	}
	else {
		fprintf(hwregtrace_file, "000");
	}
	fprintf(hwregtrace_file, "%s\n", io_reg);
}


void print_leds(Command* cmd, int* registers, int* io_registers, FILE* leds_file) {
	int io_reg_num = registers[cmd->rs] + registers[cmd->rt];
	char leds_hex[LEDS_HEX + 1];
	if (io_reg_num == LEDS && cmd->opcode == OUT) {
		fprintf(leds_file, "%d ", io_registers[CLKS] - 1);
		dec_to_n_chars_hex(leds_hex, io_registers[LEDS], LEDS_HEX);
		fprintf(leds_file, "%s\n", leds_hex);

	}
}

void print_display7seg(Command* cmd, int* registers, int* io_registers, FILE* display7seg_file) {
	int io_reg_num = registers[cmd->rs] + registers[cmd->rt];
	char dispaly7seg_hex[LEDS_HEX + 1];
	if (io_reg_num == DISPLAY7SEG && cmd->opcode == OUT) {
		fprintf(display7seg_file, "%d ", io_registers[CLKS] - 1);
		dec_to_n_chars_hex(dispaly7seg_hex, io_registers[DISPLAY7SEG], LEDS_HEX);
		fprintf(display7seg_file, "%s\n", dispaly7seg_hex);

	}
}

void print_diskout_file(hard_disk* hd, char** argv) {
	FILE* diskout_file = NULL;
	char line[LINE_SIZE];
	fopen_s(&diskout_file, argv[11], "w");
	for (int row = 0; row < SECTOR_SIZE; row++) {
		for (int col = 0; col < SECTOR_SIZE; col++) {
			if (diskout_file) {
				dec_to_n_chars_hex(line, hd->memmory[row][col], LINE_SIZE - 1);
				fprintf(diskout_file, "%s\n", line);
			}
		}
	}
	if (diskout_file) {
		fclose(diskout_file);
	}
}
