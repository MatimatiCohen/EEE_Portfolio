#ifndef SHARED_WORK_QUEUE_H
#define SHARED_WORK_QUEUE_H


//shared work queue module

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "utils.h"
#include "commands.h"

#define MAX_LINE_WIDTH 1024

//structures
// A linked list node to store a queue entry
struct SWQNode 
{
    JobLineQueue jlq; 
    struct SWQNode* next;
    char cmd_line[MAX_LINE_WIDTH+1];
    time_t read_line_time;
};
typedef struct SWQNode SWQNode;

// The queue, front stores the front node of LL and rear
// stores the last node of LL
struct SharedWorkQueue {
    SWQNode *front;
    SWQNode *rear;
    int count;
};
typedef struct SharedWorkQueue SharedWorkQueue;

//Shared Queue functions
SharedWorkQueue* createSharedWorkQueue();
void enSharedWorkQueue(SharedWorkQueue* swq, JobLineQueue *jl, char *line, time_t read_line_time);
SWQNode* deSharedWorkQueue(SharedWorkQueue* swq);

//parse job line to shared queue
void create_job_line(char *line,SharedWorkQueue *swq, pthread_mutex_t *mutex, time_t read_line_time);
int check_basic_command(char *bc);

#endif