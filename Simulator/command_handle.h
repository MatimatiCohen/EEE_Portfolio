#ifndef COMMAND_HANDLE_H
#define COMMAND_HANDLE_H

#include <stdio.h>
#include <stdbool.h>

#define LINE_SIZE 6

typedef struct Command {
	int opcode;
	int rt;
	int rd;
	int rs;
	int format;
	int imm;
}Command;



typedef struct GlobalParam {
	int clk;
	int pc;
	int halted;
	int interupt_in_proccess;
	int irq;
}GP;



void initialize_global_params(GP *gp);
void initialize_main_memory(char* argv[], char main_memory[][LINE_SIZE]);
void initialize_command(Command* cmd);
bool is_hex_negative(char* hex_str);
void decode(char main_memory[][LINE_SIZE], Command* cmd, int* registers_arr, int* IO_regirsters_arr, GP* gp);
void update_command(Command* cmd, char* opcode, char* rd, char* rs, char* rt);
void execute_command(char main_memory[][LINE_SIZE], Command* cmd,int *registers_arr,int *IO_regirsters_arr, GP* gp,FILE *hwregtrace_file);
void increment_gp(Command* cmd, GP* gp, int *io_registers);
int hex_to_char(int dec);
void dec_to_n_chars_hex(char* hex_char, int dec, int n);


#endif