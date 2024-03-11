#include <stdio.h>
#include "pass2.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LABEL 50

// TODO:
// 1) Add "define" with global constants

//---------------------------Functions--------------------------------//
void second_pass(FILE* f, char* argv[], label labels_array[], int label_count) {
	char line[MAX_ROW];
	command cmd;
	int line_num = 0;
	// Place default values in cmd
	initialize_command(&cmd);

	FILE* out_f = NULL;
	FILE* temp_out_f = NULL;
	if (argv[2]) {
		out_f = &argv[2];
		temp_out_f = &argv[2];
	}
	else {
		printf("\nError: couldn't find the memin file\n");
	}
	
	fopen_s(&f, argv[1], "r");       // input file to read from
	fopen_s(&(out_f), argv[2], "w"); // output file to write to

	assert(f != NULL);
	assert(out_f != NULL);
	// TODO: change MaxCount to default size in the while loop
	while (fgets(line, sizeof(line), f) != NULL) {
		// If the line start with '#' or is empty
		if (!check_line_valid(line, 0)) {
			continue;
		}
		parse_line(line, &cmd, labels_array, 2, label_count);
		print_line(out_f, &cmd,  &line_num);
		//printf("the command is: %s\nthe command type is %s\n\n", cmd.opcode, cmd.type);
	}
	fclose(f);
}

void initialize_command(command* cmd) {
	errno_t buffer = 2;
	strcpy_s(cmd->imm, buffer, "N");
	strcpy_s(cmd->opcode, buffer, "N");
	strcpy_s(cmd->label, buffer, "N");
	strcpy_s(cmd->rd, buffer, "N");
	strcpy_s(cmd->rs, buffer, "N");
	strcpy_s(cmd->rt, buffer, "N");
	strcpy_s(cmd->type, buffer, "N");
	strcpy_s(cmd->word_address, buffer, "N");
	strcpy_s(cmd->word_val, buffer, "N");
}

bool parse_line(char* line, command* cmd, label labels_array[], int pass_num, int label_count)
{
	char opcode[REG_SIZE] = "NONE";
	char reg1[REG_SIZE] = "NONE";
	char reg2[REG_SIZE] = "NONE";
	char reg3[REG_SIZE] = "NONE";
	char temp[REG_SIZE] = "NONE";
	int arg_num = 0;
	// remove commas and comments
	int label_and_command = clean_line(line, pass_num);
	arg_num = sscanf_s(line, 
						"%s %s %s %s %s",
						&opcode, 
					    (unsigned)_countof(opcode),
						&reg1,
						(unsigned)_countof(reg1),
						&reg2,
						(unsigned)_countof(reg2),
						&reg3,
						(unsigned)_countof(reg3),
						&temp,
						(unsigned)_countof(temp));
	
	update_command_type(cmd, line, arg_num, opcode, reg1, reg2, reg3);

	update_command(cmd, opcode, reg1, reg2, reg3, temp);
	// In case we are in the second pass, we need to replace the lables with the coresponding address
	if (pass_num == 2) {
		if (is_cmd_i_type(cmd)) {
			update_imm(cmd, labels_array, label_count);
		}
	}
	return label_and_command;
}
// return true if the line is both a label and a command
bool clean_line(char* line, int pass_num) {
	int i = 0;
	while (line[i] != '\0') {
		if (line[i] == ',') {
			line[i] = ' ';
		}
		if (line[i] == '#') {
			line[i] = '\0';
			break;
		}
		i++;
	}

	// If the line is a label followed by a command, we need to only regard the command in the second pass
	// for example:
	//				 fib: add $sp, $sp, $imm, -3
	// will become: 
	//				 add $sp, $sp, $imm, -3
	int flag = false;
	i = 0;
	int j = 0;
	while (line[i] != '\0') {
		if (line[i] == ':') {
			++i;
			while (line[i] == ' ' || line[i] == '\t') {
				i++;
			}
			if (line[i] != '\0' & line[i] != '\n' & line != '\t') {
				while (line[i] != '\0' & line[i] != '\n' & line[i] != '\t') {
					flag = true;
					line[j] = line[i];
					j++;
					i++;
				}
				line[j] = '\0';

			}
		}
		i++;
	}
	return flag;
}

int register_number(char* reg) {
	int a0 = 3, t0 = 7, s0 = 10, v0 = 2;
	if (reg[0] == 'N') {
		return -1;
	}
	assert(reg[0] == '$');

	if (reg[1] == 'a') {
		return (a0 + (reg[2] - '0'));
	}
	else if (reg[1] == 't') {
		return (t0 + (reg[2] - '0'));
	}
	else if ((reg[1] == 's') && (reg[2] != 'p')) {
		return (s0 + (reg[2] - '0'));
	}
	else if (reg[1] == 'v') {
		return v0;
	}
	else if (!strcmp(reg, "$zero")) {
		return 0;
	}
	else if (!strcmp(reg, "$imm")) {
		return 1;
	}
	else if (!strcmp(reg, "$gp")) {
		return 13;
	}
	else if (!strcmp(reg, "$sp")) {
		return 14;
	}
	else if (!strcmp(reg, "$ra")) {
		return 15;
	}
	else {
		printf("ERROR: register_number -  didn't recognize register: %s\n", reg);
		return -1;
	}
}

int opcode_number(char* opcode) {
	if (!strcmp(opcode, "add")) {
		return 0;
	}
	else if (!strcmp(opcode, "sub")) {
		return 1;
	}
	else if (!strcmp(opcode, "mul")) {
		return 2;
	}
	else if (!strcmp(opcode, "and")) {
		return 3;
	}
	else if (!strcmp(opcode, "or")) {
		return 4;
	}
	else if (!strcmp(opcode, "xor")) {
		return 5;
	}
	else if (!strcmp(opcode, "sll")) {
		return 6;
	}
	else if (!strcmp(opcode, "sra")) {
		return 7;
	}
	else if (!strcmp(opcode, "srl")) {
		return 8;
	}
	else if (!strcmp(opcode, "beq")) {
		return 9;
	}
	else if (!strcmp(opcode, "bne")) {
		return 10;
	}
	else if (!strcmp(opcode, "blt")) {
		return 11;
	}
	else if (!strcmp(opcode, "bgt")) {
		return 12;
	}
	else if (!strcmp(opcode, "ble")) {
		return 13;
	}
	else if (!strcmp(opcode, "bge")) {
		return 14;
	}
	else if (!strcmp(opcode, "jal")) {
		return 15;
	}
	else if (!strcmp(opcode, "lw")) {
		return 16;
	}
	else if (!strcmp(opcode, "sw")) {
		return 17;
	}
	else if (!strcmp(opcode, "reti")) {
		return 18;
	}
	else if (!strcmp(opcode, "in")) {
		return 19;
	}
	else if (!strcmp(opcode, "out")) {
		return 20;
	}
	else if (!strcmp(opcode, "halt")) {
		return 21;
	}
	else {
		printf("ERROR: opcode_number - didn't recognize opcode: %s\n", opcode);
		return -1;
	}
}

// Check if the register is IMM type
bool is_reg_imm(char* reg) {
	if (!strcmp(reg, "$imm")) {
		return true;
	}
	return false;
}

// Update the cmd's instruction type - (R,I,J,L,O)
void update_command_type(command* cmd, char* line, int arg_num, char* opcode, char* reg1, char* reg2, char* reg3) {
	errno_t buffer = 2;
	// Check if the command is a Lable type
	if (find_if_label_in_line(line)) {
		strcpy_s(cmd->type, buffer, "L");
	}
	// Check if the command is an I type
	else if (is_reg_imm(reg1) || is_reg_imm(reg2) || is_reg_imm(reg3)) {
		strcpy_s(cmd->type, buffer, "I");
	}
	else if (is_cmd_dotword(line)) {
		strcpy_s(cmd->type, buffer, "O");
	}
	// if the command is an R type
	else{
		strcpy_s(cmd->type, buffer, "R");
	}
}

bool find_if_label_in_line(char* line) {


	int i = 0;
	while (line[i] != '\0') {
		if (line[i] == '#') {
			return false;
		}
		if (line[i] == ':') {
			return true;
		}
		i++;
	}
	return false;
}




bool is_cmd_dotword(char* line)
{
	int i = 0;
	while (line[i] != '\0' & line[i] != '#') {
		if (line[i] == '.') {
			return true;
		}
		i++;
	}
	return false;
}

// Update the opcode, registers and labels
void update_command(command* cmd, char* opcode, char* reg1, char* reg2, char* reg3, char* temp) {
	errno_t buffer = (errno_t)sizeof(cmd->opcode);
	// If it's just a label, skip the line
	if (!strcmp(cmd->type, "L")) {
		return;
	}
	if (!strcmp(cmd->type, "O")) {
		strcpy_s(cmd->word_address, buffer, reg1);
		strcpy_s(cmd->word_val, buffer, reg2);
		strcpy_s(cmd->opcode, buffer, opcode);
		//printf("word address to print: %s\n", cmd->word_address);
		//printf("word value to print: %s\n", cmd->word_val);
		return;
	}
	strcpy_s(cmd->opcode, buffer, opcode);
	strcpy_s(cmd->rd, buffer, reg1);
	strcpy_s(cmd->rs, buffer, reg2);
	strcpy_s(cmd->rt, buffer, reg3);

	// if the command is an I type
	if (is_cmd_i_type(cmd)) {
		strcpy_s(cmd->imm, buffer, temp);
	}
}

// Update the IMM value to hex 
void update_imm(command* cmd, label* labels_array, int label_count) {
	errno_t buffer = ADDRESS_LEN + 1;
	int i = 0;
	while (i<label_count) {
		if (!strcmp(cmd->imm, labels_array[i].name)) {
			//printf("replacing lable %s with address %s\n",labels_array[i].name, labels_array[i].address);
		  	strcpy_s(&cmd->imm, buffer, labels_array[i].address);
			return;
		} 
		i++;
	}
	// In case the imm given is a simple number and not a label
	int dec = -1;
	char temp_imm[MAX_LABEL];
	if (is_hex(cmd->imm)) {
		dec= (int)strtol(cmd->imm+2, NULL, 16);
	}
	else {
		dec = atoi(cmd->imm);
	}
	dec_to_n_chars_hex(temp_imm, dec, ADDRESS_LEN);
	strcpy_s(&cmd->imm, buffer, temp_imm);
}

bool is_hex(char* str) {
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		return true;
	}
	return false;
}

// Print the line in the memory file
void print_line(FILE* out_f, command* cmd, int* line_num) {
	if (!strcmp(cmd->type, "L") ) {
		return;
	}
	// If the command is .word
	if (!strcmp(cmd->type, "O")) {
		// TODO: Fix the issue: when using .word command in the middle of the file - it doesn't work
		print_word(out_f, cmd, line_num);
		return;
	}
	int opcode;
	int rd;
	int rs;
	int rt;
	char opcode_hex[3];
	char rd_hex[2];
	char rs_hex[2];
	char rt_hex[2];
	
	opcode = opcode_number(cmd->opcode);
	rd = register_number(cmd->rd);
	rs = register_number(cmd->rs);
	rt = register_number(cmd->rt);

	dec_to_n_chars_hex(opcode_hex, opcode, 2);
	dec_to_n_chars_hex(rd_hex, rd, 1);
	dec_to_n_chars_hex(rs_hex, rs, 1);
	dec_to_n_chars_hex(rt_hex, rt, 1);

	fprintf(out_f,"%s", opcode_hex);
	fprintf(out_f,"%s", rd_hex);
	fprintf(out_f,"%s", rs_hex);
	fprintf(out_f,"%s", rt_hex);
	fprintf(out_f,"\n");
	(*line_num)++;
	// in case the command is an IMM type:
	if (is_cmd_i_type(cmd)) {
		fprintf(out_f, "%s\n", cmd->imm);
		(*line_num)++;
	}

}

// TODO: need to handle edge cases (hex input, capital letters etc.)
// Print the .word command
void print_word(FILE* out_f, command* cmd, int* line_num) {
	
	long saved_address= ftell(out_f); // Save the current place in the file so it can later be resotored
	
	int line_num_to_write = -1;
	int dec_num_to_print = -1;
	char num_to_print[ADDRESS_LEN+1];
	
	if (is_hex(cmd->word_address)) {
		line_num_to_write = (int)strtol(cmd->word_address + 2, NULL, 16);
	}
	else {
		line_num_to_write = atoi(cmd->word_address);
	}
	
	if (is_hex(cmd->word_val)) {
		dec_num_to_print = (int)strtol(cmd->word_val + 2, NULL, 16);
	}
	else {
		dec_num_to_print = atoi(cmd->word_val);
	}

	dec_to_n_chars_hex(num_to_print, dec_num_to_print, ADDRESS_LEN);

	//Go to the last line of the file
	fseek(out_f, 0, SEEK_END);

	// Fill with zeros until our line if the lines are empty
	int flag = 0;
	while (line_num_to_write > *line_num) {
		flag = 1;
		fprintf(out_f, "%s", "00000\n");
		(*line_num)++;
	}
	if (flag == 1) {
		(*line_num)++;
	}
	
	fseek(out_f, 0, SEEK_SET);
	// Go to the line we need to print
	fseek(out_f, 7*sizeof(char)*(line_num_to_write), SEEK_SET);

	fprintf(out_f, "%s\n", num_to_print);

	// Return to the place in the file before inserting the new word 
	fseek(out_f, saved_address, SEEK_SET);
	
}

// Return True if the command is an I type command
bool is_cmd_i_type(command* cmd) {
	if (!strcmp(cmd->type, "I")) return true;
	return false;
}
//------------------------------------------------------------------//
