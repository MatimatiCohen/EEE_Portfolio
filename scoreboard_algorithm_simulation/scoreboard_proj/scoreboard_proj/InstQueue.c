#include "InstQueue.h"

#define QUEUE_MAX_SIZE 16

void InitQueue(InstQueue* inst_queue) {
    inst_queue->front = -1;
    inst_queue->rear = -1;
}

// Check if the queue is full
int isFull(InstQueue *inst_queue) {
    if ((inst_queue->front == inst_queue->rear + 1) || (inst_queue->front == 0 && inst_queue->rear == QUEUE_MAX_SIZE - 1)) return 1;
    return 0;
}

// Check if the queue is empty
int isEmpty(const InstQueue inst_queue) {
    if (inst_queue.front == -1) return 1;
    return 0;
}

// Adding an element
void enQueue(InstQueue *inst_queue,Inst *instruction) {
    if (isFull(inst_queue))
        return;
    else {
        if (inst_queue->front == -1) inst_queue->front = 0;
        inst_queue->rear = (inst_queue->rear + 1) % QUEUE_MAX_SIZE;
        copy_inst(instruction, &inst_queue->instructions[inst_queue->rear]);
    }
}

void copy_inst(Inst* source, Inst* Dest) {
    Dest->dst= source->dst;
    Dest->imm= source->imm ;
    Dest->op= source->op  ;
    Dest->src0=source->src0  ;
    Dest->src1=  source->src1 ;
    Dest->zero=  source->zero ;
    Dest->pc = source->pc;
}

// Removing an element
Inst deQueue(InstQueue *inst_queue) {
    Inst inst= inst_queue->instructions[inst_queue->front];
    if (inst_queue->front == inst_queue->rear) {
        inst_queue->front = -1;
        inst_queue->rear = -1;
    }
    else {
        inst_queue->front = (inst_queue->front + 1) % QUEUE_MAX_SIZE;
    }
    return inst;    
}

void CreateInst(Inst* instruction, uint32_t mem_word,int pc) {
    uint32_t zero = mem_word >> 28;  // bits [31:28]
    uint32_t op = mem_word << 4;    // bits [27:24]
    op = op >> 28;
    uint32_t dest = mem_word << 8;   // bits [23:20]
    dest = dest>>28;
    uint32_t src0 = mem_word << 12; // bits [19:16]
    src0 = src0 >> 28;
    uint32_t src1 = mem_word << 16;  // bits [15:12]
    src1 = src1 >> 28;
    uint32_t imm = mem_word << 20;  // bits [11:0]
    imm = imm >> 20;

    instruction->pc = pc;
    instruction->zero = zero;
    instruction->op = op;
    instruction->dst = dest;
    instruction->src0 = src0;
    instruction->src1 = src1;
    instruction->imm = imm;
}

int current_opcode(InstQueue* inst_queue) {
    return inst_queue->instructions[inst_queue->front].op;
}