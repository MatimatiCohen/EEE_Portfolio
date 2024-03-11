#ifndef FILES_PRINTER_H
#define FILES_PRINTER_H

#include "IO_handle.h"

void print_leds(Command* cmd, int* registers, int* io_registers, FILE* leds_file);
void print_display7seg(Command* cmd, int* registers, int* io_registers, FILE* display7seg_file);
void print_monitor(Monitor* moni, char** argv);
void print_cycles_file(int* io_registers, char** argv);
void print_main_memmory(char main_memmory[][LINE_SIZE], char** argv);
void print_regout_file(int* registers, char** argv);
void print_trace_file(Command* cmd, int* registers, GP* gp, FILE* trace_file);
void print_hwregrace_file(Command* cmd, int* registers, int* io_registers, FILE* hwregtrace_file);
void print_diskout_file(hard_disk* hd, char** argv);


#endif