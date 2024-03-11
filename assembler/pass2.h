#ifndef PASS2_H
#define PASS2_H

#include "pass1.h"

#define OPCODE_SIZE 2
#define REG_SIZE 50
#define TYPE_SIZE 9

typedef struct Command {
	char opcode[REG_SIZE];
	char rd[REG_SIZE];
	char rt[REG_SIZE];
	char rs[REG_SIZE];
	char label[MAX_LABEL];
	char type[TYPE_SIZE];
	char imm[REG_SIZE];
	char word_address[MAX_LABEL];
	char word_val[MAX_LABEL];
}command;

//-------------------------Declerations-------------------------------//
void second_pass(FILE* f, char* argv[], label labels_array[], int label_count);
void initialize_command(command* cmd);
bool parse_line(char* line, command* cmd, label labels_array[], int pass_num, int label_count);
bool clean_line(char* line, int pass_num);
bool find_if_label_in_line(char* line);
bool is_cmd_dotword(char* line);
int register_number(char* reg);
int opcode_number(char* opcode);
bool is_reg_imm(char* reg);
void update_command_type(command* cmd, char* line, int arg_num, char* opcode, char* reg1, char* reg2, char* reg3);
void update_command(command* cmd, char* opcode, char* reg1, char* reg2, char* reg3, char* temp);
void update_imm(command* cmd, label* labels_array, int label_count);
bool is_hex(char* str);
void print_line(FILE* out_f, command* cmd, int* line_num);
void print_word(FILE* out_f, command* cmd, int* lines_num);
bool is_cmd_i_type(command* cmd);
//--------------------------------------------------------------------//


#endif 

