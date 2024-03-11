
#include <stdio.h>
#include <assert.h>
#include "command_handle.h"
#include "IO_handle.h"


#define LINE_SIZE 6
#define MEM_SIZE 4096
#define NUM_OF_REGS 16
#define NUM_OF_IO_REGS 23


int main(int argc, char* argv[]) {

	// Initialize area: 

	FILE* output_file = NULL;
	FILE* trace_file = NULL;
	FILE* irq2in_file = NULL;
	FILE* hwregtrace_file = NULL;
	FILE* leds_file = NULL;
	FILE* display7seg_file = NULL;
	fopen_s(&trace_file, argv[6], "w");
	assert(trace_file != NULL);
	fopen_s(&hwregtrace_file, argv[7], "w");
	assert(hwregtrace_file != NULL);
	fopen_s(&irq2in_file, argv[3], "r");
	assert(irq2in_file != NULL);
	fopen_s(&leds_file, argv[9], "w");
	assert(leds_file != NULL);
	fopen_s(&display7seg_file, argv[10], "w");
	assert(display7seg_file != NULL);
	irq2_manage irq2;
	initialize_irq2_manage(&irq2);
	Command cmd;
	hard_disk hd;
	char main_memory[MEM_SIZE][LINE_SIZE];
	int registers_arr[NUM_OF_REGS] = { 0 }; //initialize registers
	int IO_registers_arr[NUM_OF_IO_REGS] = { 0 }; //initalize IO registers
	GP global_params;
	initialize_global_params(&global_params);
	initialize_main_memory(argv, main_memory);
	initialize_hard_disk(&hd, argv);
	Monitor monitor;
	init_frame_buffer(&monitor);
	int i = 0;

	//Fetch Decode Execute loop:

	while (!global_params.halted) {  
		read_irq2in_data(&irq2, irq2in_file);
		set_irq_value(&global_params, IO_registers_arr);
		if (global_params.irq && !global_params.interupt_in_proccess) {
			allow_interupt_begin(&global_params, IO_registers_arr);
		}
		initialize_command(&cmd);
		decode(main_memory, &cmd, registers_arr, IO_registers_arr, &global_params);
		print_trace_file(&cmd, registers_arr, &global_params, trace_file);
		execute_command(main_memory, &cmd, registers_arr, IO_registers_arr, &global_params, hwregtrace_file);
		hard_disk_operation(&hd, IO_registers_arr,main_memory);
		set_irq2_status(&irq2, IO_registers_arr);
		set_frame_buffer(&monitor, IO_registers_arr, &global_params);
		timer_update(IO_registers_arr);
		print_leds(&cmd, registers_arr, IO_registers_arr, leds_file);
		print_display7seg(&cmd, registers_arr, IO_registers_arr, display7seg_file);
}
	

	
	// Output files area:

	print_monitor(&monitor, argv); 
	print_cycles_file(IO_registers_arr, argv); 
	print_main_memmory(main_memory, argv); 
	print_regout_file(registers_arr, argv); 
	print_diskout_file(&hd, argv);

	// Close files:
	
	if (output_file) {
		fclose(output_file);
	}
	if (trace_file) {
		fclose(trace_file);
	}
	if (hwregtrace_file) {
		fclose(hwregtrace_file);
	}
	if (irq2in_file) {
		fclose(irq2in_file);
	}
	if (leds_file) {
		fclose(leds_file);
	}
	if (display7seg_file) {
		fclose(display7seg_file);
	}
	return 0;
}



