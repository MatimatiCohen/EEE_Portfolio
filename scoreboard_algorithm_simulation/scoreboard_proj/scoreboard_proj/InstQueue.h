
#ifndef INSTQUEUE_H
#define INSTQUEUE_H

#include <inttypes.h>

#define QUEUE_MAX_SIZE 16
#define WORD 32

enum opcode{LD,ST,ADD,SUB,MUL,DIV,HALT};

struct Instruction{
	int zero; // bits [31:28] 
	int op; // ADD,MUL,SUB,DIV,LD,ST
	int dst;    // 0-15
	int src0;
	int src1;
	int imm;    // 000-FFF (can be easily convertes to int with strtol(hexstring,NULL,16)   
	int pc;
};
typedef struct Instruction Inst;

struct InstructionQueue {
	Inst instructions[QUEUE_MAX_SIZE];
	int front;
	int rear;
};
typedef struct InstructionQueue InstQueue;

void CreateInst(Inst* instruction, uint32_t mem_word, int pc);
void InitQueue(InstQueue* inst_queue);
int isEmpty(const InstQueue inst_queue);
void enQueue(InstQueue *inst_queue,Inst *instruction);
Inst deQueue(InstQueue *inst_queue);
void copy_inst(Inst* source, Inst* Dest);
int isFull(InstQueue* inst_queue);
int current_opcode(InstQueue* inst_queue);

#endif