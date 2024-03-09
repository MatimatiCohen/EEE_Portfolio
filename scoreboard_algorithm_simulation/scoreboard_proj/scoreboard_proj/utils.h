
#ifndef UTILS_H
#define UTILS_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "MemResources.h"

#define NUM_OF_ARGS 7
#define CONFIG_FILE_ARG 1
#define MEMIN_FILE_ARG 2
#define MEMOUT_FILE_ARG 3
#define REGOUT_FILE_ARG 4
#define TRACEINST_FILE_ARG 5
#define TRACEUNIT_FILE_ARG 6

#define MEM_FILE_LINE_WIDTH 8
#define MAX_CONF_LINE_LEN 17 //longest parameter and value=16
#define NUM_CONF_FIELDS 13
#define	NUM_REGS 16

struct ConfField {
	char parameter[MAX_CONF_LINE_LEN];
	char value_str[MAX_CONF_LINE_LEN];
};
typedef struct ConfField ConfField;

struct UserConf {
	int add_nr_units;
	int sub_nr_units;
	int mul_nr_units;
	int div_nr_units;
	int ld_nr_units;
	int st_nr_units;
	int add_delay;
	int sub_delay;
	int mul_delay;
	int div_delay;
	int ld_delay;
	int st_delay;
	char trace_unit[MAX_CONF_LINE_LEN];
};
typedef struct UserConf UserConf;

//Parse Data functions
int GetArgs(ConfField* conf_fields, UserConf* user_conf, Mem* mem, int argc, char** args);

//Config functions
void SetConfig(ConfField* conf_fields, UserConf* user_conf, FILE* config_file);

//pure utils
size_t getline(char** lineptr, size_t* n, FILE* stream);
void dec_to_n_chars_hex(char* hex_char, int dec, int n); // Convert a decimal number to an n-sized char in hex 
int hex_to_char(int dec);

#endif