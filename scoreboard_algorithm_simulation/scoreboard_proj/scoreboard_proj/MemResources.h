
#ifndef MEMRESOURCES_H
#define MEMRESOURCES_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <inttypes.h>

#define MEM_LEN 4096

union F2U {
	float f;
	uint32_t u;
};
typedef union F2U F2U;

struct Mem {
	F2U mem_data[MEM_LEN];
};
typedef struct Mem Mem;

struct Reg {
	float val;
	char tag[5]; //indicates which functional unit will write to the register
};
typedef struct Reg Reg;

//Regs functions
void InitRegs(Reg* regs);
int PrintRegs(Reg* regs, char** args);
int RegHasTag(Reg reg);
float GetRegData(Reg* regs, char* reg_name);
int GetRegNum(Reg* regs, char* reg_name);

//Mem functions
void InitMem(Mem* mem, FILE* mem_in_file);
int PrintMem(Mem* mem, char** args);

#endif