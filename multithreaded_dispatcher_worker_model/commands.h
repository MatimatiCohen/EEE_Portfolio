#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <pthread.h>
#include "utils.h"

//module for dispatcher and thread commands 


//structures

// dispatcher commands
struct DispatcherCommands{
    char *cmd;
    int cmd_val;
    int wait_flag;
    int worker_flag;
};
typedef struct DispatcherCommands DispatcherCommands;


//structs for worker thread job line queue

// data of queue node
struct node_data
{
    int cmd_flag;
    int cmd_arg;
};
typedef struct node_data node_data;

//node for job line queue
struct node
{
    node_data nd;
    struct node *next;
};
typedef struct node node;


//queue in order to build a worker command line
struct JobLineQueue
{
    int count;
    node *front;
    node *rear;
};
typedef struct JobLineQueue JobLineQueue;


// Dispatcher parse line functions

void DispatcherCommandInit(DispatcherCommands *dc);
void DispatcherCommandUpdate(DispatcherCommands *dc,char* cmd_line);
int DispatcherCmdExecuter(DispatcherCommands *dc,UserArgs *user_args,pthread_t *tid);


// job line queue functions
bool is_worker_cmd(char* string);
void initialize(JobLineQueue *q);
void enqueue(JobLineQueue *q, node_data nd);
node* dequeue(JobLineQueue *q);
void init_node_nd(node_data *nd);
void update_node_data(node_data* nd, int basic_check, int arg );
void destruct_queue(JobLineQueue* q);
int isempty(JobLineQueue  *q);

#endif