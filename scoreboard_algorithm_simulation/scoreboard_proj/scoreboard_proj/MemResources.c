#include "MemResources.h"
#include "utils.h"

// Registers Functions
void InitRegs(Reg* regs) {
	for (int i = 0; i < NUM_REGS; i++) {
		regs[i].val = (float)i;
		strcpy(regs[i].tag, "--"); //initially the register tag is empty
	}
}

int RegHasTag(Reg reg) {
	if (!strcmp(reg.tag, "--")) //if no tag 
		return 0;
	return 1;
}

int PrintRegs(Reg* regs, char** args) {
	FILE* regout = fopen(args[REGOUT_FILE_ARG], "w+");

	if (regout == NULL) return 0;

	for (int i = 0; i < NUM_REGS; i++) {
		fprintf(regout, "%0f\n", regs[i].val);
	}
	fclose(regout);
	return(1);
}

// Memory Functions
void InitMem(Mem* mem, FILE* mem_in_file)
{
	char line[MEM_FILE_LINE_WIDTH + 1];
	int line_num = 0;

	while (fgets(line, MEM_FILE_LINE_WIDTH + 1, mem_in_file))
	{
		int tmp = fgetc(mem_in_file); //to get rid of the \n
		mem->mem_data[line_num].u = strtol(line, NULL, 16); // convert line to int
		line_num++;
	}

	while (line_num < MEM_LEN) //for case where the memin file is less than 4096 lines
	{
		mem->mem_data[line_num].u = 0;
		line_num++;
	}
}

int PrintMem(Mem* mem, char** args)
{
	int highest_no_zero = MEM_LEN - 1;
	while (mem->mem_data[highest_no_zero].f == 0.0) //to find the maximal mem cell which is not zero
	{
		highest_no_zero--;
	}

	FILE* memout = fopen(args[MEMOUT_FILE_ARG], "w+");
	if (memout == NULL) return 0;

	for (int i = 0; i <= highest_no_zero; i++)
	{
		fprintf(memout, "%08x\n", mem->mem_data[i].u);
	}
	fclose(memout);

	return(1);
}

float GetRegData(Reg* regs, char* reg_name)
{
	int reg_num = atoi(&reg_name[1]); //first char in reg_name is always F
	return (regs[reg_num].val);
}

int GetRegNum(Reg* regs, char* reg_name)
{
	return atoi(&reg_name[1]); //first char in reg_name is always F
}