#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "command_handle.h"
#include "IO_handle.h"



// General Prameters
#define LINE_SIZE 6
#define MEM_SIZE 4096

// Command formats
#define R_FORMAT 0
#define I_FORMAT 1
#define M_FORMAT 2 // for memmory commands (3 clocks)

// Registers (need to add them all)
#define IMM_REG 1
#define ZERO_REG 0

// opcodes
#define ADD 0
#define SUB 1
#define MUL 2
#define AND 3
#define OR  4
#define XOR 5
#define SLL 6
#define SRA 7
#define SRL 8
#define BEQ 9
#define BNE 10
#define BLT 11
#define BGT 12
#define BLE 13
#define BGE 14
#define JAL 15
#define LW  16
#define SW  17
#define RETI 18
#define IN 19
#define OUT 20
#define HALT 21

// IO_Registers

#define IRQ0ENABLE 0
#define IRQ1ENABLE 1
#define IRQ2ENABLE 2
#define IRQ0STATUS  3
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


void arith_command(Command* cmd, int* registers);
void jump_command(Command* cmd, int* registers, GP* gp);
void memory_command(char main_memory[][LINE_SIZE], Command* cmd, int* registers);
void io_command(Command* cmd, int* registers, int* io_registers, GP* gp);

void initialize_main_memory(char* argv[], char main_memory[][LINE_SIZE]) {
	FILE* input_file = NULL;
	fopen_s(&input_file, argv[1], "r");
	assert(input_file != NULL);

	int i = 0;
	errno_t buffer = LINE_SIZE;
	char line[LINE_SIZE];
	while (fgets(line, LINE_SIZE, input_file) != NULL) {
		strcpy_s(main_memory[i], buffer, line);
		getc(input_file); // Discard '\n'
		i++;
	}
	
	while (i < MEM_SIZE) {
		strcpy_s(main_memory[i], buffer, "00000");
		getc(input_file); // Discard '\n'
		i++;
	}

	if (input_file) {
		fclose(input_file);
	}


}


void initialize_command(Command* cmd) {
	cmd->opcode = -1;
	cmd->rd = -1;
	cmd->rs = -1;
	cmd->rt = -1;
	cmd->format = -1;
	cmd->imm = -1;
}

void initialize_global_params(GP *gp) {
	gp->clk = 0;
	gp->pc = 0;
	gp->halted = 0;
	gp->interupt_in_proccess = 0;
	gp->irq = 0;
}

void decode(char main_memory[][LINE_SIZE], Command* cmd, int* registers_arr, int* IO_regirsters_arr, GP* gp) {

	char opcode[3];
	char rd[2];
	char rs[2];
	char rt[2];
	memcpy(opcode, main_memory[gp->pc], 2);
	opcode[2] = '\0';
	memcpy(rd, main_memory[gp->pc] + 2, 1);
	rd[1] = '\0';
	memcpy(rs, main_memory[gp->pc] + 3, 1);
	rs[1] = '\0';
	memcpy(rt, main_memory[gp->pc] + 4, 1);
	rt[1] = '\0';
	update_command(cmd, opcode, rd, rs, rt);
	int imm = 0;
	// Initialize Imm value
	if (cmd->format == I_FORMAT) {

		imm = (int)strtol(main_memory[gp->pc + 1], NULL, 16);
		if (is_hex_negative(main_memory[gp->pc + 1])) {
			imm |= 0xfff00000;
		}
		
	}
	cmd->imm = imm;
	registers_arr[IMM_REG] = cmd->imm;
	

}


bool is_hex_negative(char* hex_str) {
	if (hex_str[0] >= '8'){
		return true;
	}
	return false;
}


void update_command(Command* cmd, char* opcode, char* rd, char* rs, char* rt) {
	cmd->opcode = (int)strtol(opcode, NULL, 16);
	cmd->rd = (int)strtol(rd, NULL, 16);
	cmd->rs = (int)strtol(rs, NULL, 16);
	cmd->rt = (int)strtol(rt, NULL, 16);
	if ((cmd->rd == IMM_REG) || (cmd->rs == IMM_REG) || cmd->rt == IMM_REG) {
		cmd->format = I_FORMAT;
	}
	else {
		cmd->format = R_FORMAT;
	}
}


void execute_command(char main_memory[][LINE_SIZE], Command* cmd, int* registers_arr, int* IO_regirsters_arr, GP* gp,FILE* hwregtrace_file) {

	increment_gp(cmd, gp, IO_regirsters_arr);
	if (cmd->opcode >= ADD && cmd->opcode <= SRL ) {
		arith_command(cmd, registers_arr);
	}
	else if (cmd->opcode >= BEQ && cmd->opcode <= JAL) {
		jump_command(cmd, registers_arr, gp);
	}
	else if (cmd->opcode == LW || cmd->opcode == SW) {
		memory_command(main_memory, cmd, registers_arr);
	}
	else if (cmd->opcode >= RETI && cmd->opcode <= OUT) {
		io_command(cmd, registers_arr, IO_regirsters_arr, gp);
		if (cmd->opcode > RETI && cmd->opcode <= OUT) {
			print_hwregrace_file(cmd, registers_arr, IO_regirsters_arr, hwregtrace_file);
		}
	}

	else if (cmd->opcode == HALT) {
		gp->halted = 1;
	}
	else {
		printf("\nPAY ATTENTION: command (index number - %d) is not yet supported\n",cmd->opcode);
	}
	

}


void increment_gp(Command* cmd, GP* gp,int *io_registers) {// TODO: Replace the gp->clk to io_registers[clk]
	if (cmd->format == R_FORMAT) {
		gp->pc++;
		gp->clk++;
		io_registers[CLKS]++;
		if (io_registers[CLKS] == 0xFFFFFFFF) {
			io_registers[CLKS] = 0;
		}
	}

	if (cmd->format == I_FORMAT) {
		gp->pc += 2;
		gp->clk += 2;
		io_registers[CLKS] += 2;
		if (io_registers[CLKS] == 0xFFFFFFFF) {
			io_registers[CLKS] = 0;
		}
	}
	// In case the command is lw or sw (and so takes three clock cycles)
	if (cmd->opcode == LW || cmd->opcode == SW) {
		gp->clk++;
		io_registers[CLKS]++;
		if (io_registers[CLKS] == 0xFFFFFFFF) {
			io_registers[CLKS] = 0;
		}
	}
	return;
}


void arith_command(Command* cmd, int* registers) {
	// avoid writing to $IMM or $ZERO registers
	if (cmd->rd == ZERO_REG || cmd->rd == IMM_REG) {
		return;
	}
	switch (cmd->opcode) {
	case ADD:
		registers[cmd->rd] = registers[cmd->rs] + registers[cmd->rt];
		break;
	case SUB:
		registers[cmd->rd] = registers[cmd->rs] - registers[cmd->rt];
		break;
	case MUL:
		registers[cmd->rd] = registers[cmd->rs] * registers[cmd->rt];
		break;
	case AND:
		registers[cmd->rd] = registers[cmd->rs] & registers[cmd->rt];
		break;
	case OR:
		registers[cmd->rd] = registers[cmd->rs] | registers[cmd->rt];
		break;
	case XOR:
		registers[cmd->rd] = registers[cmd->rs] ^ registers[cmd->rt];
		break;
	case SLL:
		registers[cmd->rd] = registers[cmd->rs] << registers[cmd->rt];
		break;
	case SRA:
		registers[cmd->rd] = registers[cmd->rs] >> registers[cmd->rt];  
		break;
	case SRL:
		registers[cmd->rd] = registers[cmd->rs] >> registers[cmd->rt]; 
		break;
	default:
		printf("ERROR: not arithmetic opcode\n");
	}

}

// Jump and Branch commands
void jump_command(Command* cmd, int* registers, GP* gp) {
	switch (cmd->opcode) {
	case BEQ:
		if (registers[cmd->rs] == registers[cmd->rt]) {
			gp->pc = registers[cmd->rd];
		}
		break;
	case BNE:
		if (registers[cmd->rs] != registers[cmd->rt]) {
			gp->pc = registers[cmd->rd];
		}
		break;
	case BLT:
		if (registers[cmd->rs] < registers[cmd->rt]) {
			gp->pc = registers[cmd->rd];
		}
		break;
	case BGT:
		if (registers[cmd->rs] > registers[cmd->rt]) {
			gp->pc = registers[cmd->rd];
		}
		break;
	case BLE:
		if (registers[cmd->rs] <= registers[cmd->rt]) {
			gp->pc = registers[cmd->rd];
		}
		break;
	case BGE:
		if (registers[cmd->rs] >= registers[cmd->rt]) {
			gp->pc = registers[cmd->rd];
		}
		break;
	case JAL:
		registers[cmd->rd] = gp->pc;
		gp->pc = registers[cmd->rs];
		break;
	default:
		printf("ERROR: not jump opcode\n");
	}
}


void memory_command(char main_memory[][LINE_SIZE], Command* cmd, int* registers) {

	int imm = 0;
	switch (cmd->opcode) {
	case LW:
		imm = (int)strtol(main_memory[registers[cmd->rt] + registers[cmd->rs]], NULL, 16);
		if (is_hex_negative(main_memory[registers[cmd->rt] + registers[cmd->rs]])) {
			imm |= 0xfff00000;
		}
		registers[cmd->rd] = imm;
		break;
	case SW:
		dec_to_n_chars_hex(main_memory[registers[cmd->rt] + registers[cmd->rs]], registers[cmd->rd], LINE_SIZE-1);
		break;
	default:
		printf("ERROR: not memory opcode\n");
	}
}

void io_command(Command* cmd, int* registers, int* io_registers, GP* gp) {
	int io_reg_num= registers[cmd->rs] + registers[cmd->rt];
	switch (cmd->opcode) {
	case RETI:
		gp->pc = io_registers[IRQRETURN];
		gp->interupt_in_proccess = 0;
		break;
	case IN:
		registers[cmd->rd] = io_registers[io_reg_num];
		if (io_reg_num == MONITORCMD) {
			registers[cmd->rd] = 0;  
		}
	
		break;
	case OUT:
		io_registers[io_reg_num] = registers[cmd->rd];

		break;
	default:
		printf("ERROR: not IO opcode\n");
	}

}


int hex_to_char(int dec){
	if (dec < 10)
		return '0' + dec;
	return dec + 'A' - 10;
}


void dec_to_n_chars_hex(char* hex_char, int dec, int n){
	int l_dec = dec;
	int l_val = 0;
	unsigned int u_l_dec = dec;

	// for negative values
	if (dec >= 0)
	{
		for (int j = n - 1; j >= 0; j--)
		{
			l_val = l_dec % 16;
			hex_char[j] = (char)hex_to_char(l_val);
			l_dec /= 16;
		}
	}

	//convering negative number to hex using unsigned int
	else
	{
		for (int j = n - 1; j >= 0; j--)
		{
			l_val = u_l_dec % 16;
			hex_char[j] = (char)hex_to_char(l_val);
			u_l_dec /= 16;
		}
	}

	hex_char[n] = '\0'; 
}


