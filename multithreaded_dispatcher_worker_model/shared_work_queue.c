#include "shared_work_queue.h"

#define REPEAT 4


// A utility function to create a new linked list node.
SWQNode* newNode(JobLineQueue *jl)
{
    SWQNode* temp = (SWQNode*)malloc(sizeof(SWQNode));
    temp->jlq=*jl;
    temp->next = NULL;
    return temp;
}


// A utility function to create an empty SharedWorkQueue
SharedWorkQueue* createSharedWorkQueue() 
{
    SharedWorkQueue* swq= (SharedWorkQueue*)malloc(sizeof(SharedWorkQueue));
    swq->front =NULL;
    swq->rear = NULL;
    swq->count = 0;
    return swq;
}

// The function to add a key k to q
void enSharedWorkQueue(SharedWorkQueue* swq, JobLineQueue *jl, char *line, time_t read_line_time)
{
    // Create a new LL node
    SWQNode* temp = newNode(jl);

    strcpy(temp->cmd_line,line);
    temp->read_line_time = read_line_time;

    // If SharedWorkQueue is empty, then new node is front and rear
    // both
    if (swq->rear == NULL) {
        swq->front = swq->rear = temp;
        return;
    }
    // Add the new node at the end of SharedWorkQueue and change rear
    swq->rear->next = temp;
    swq->rear = temp;
    swq->count+=1;
}

// Function to remove a key from given SharedWorkQueue q
SWQNode* deSharedWorkQueue(SharedWorkQueue* swq)
{
    
    // If SharedWorkQueue is empty, return NULL.
    if (swq->front == NULL)
    {
        return(NULL);
    }

    swq->count-=1;
    // Store previous front and move front one node ahead
    SWQNode* temp = swq->front;

    swq->front = swq->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (swq->front == NULL)
        swq->rear = NULL;

    return(temp);
}

void create_job_line(char *line,SharedWorkQueue * swq, pthread_mutex_t *mutex, time_t read_line_time){
    const char s[2] = ";";
    char temp[10],bc[10];
    int arg, repeat = 0;
    char *token;
    char original_line[MAX_LINE_WIDTH+1];
    node_data nd;
    init_node_nd(&nd);
    JobLineQueue *jlq, *jlq_rep;
    jlq = malloc(sizeof(JobLineQueue));
    initialize(jlq);
    jlq_rep = malloc(sizeof(JobLineQueue));
    initialize(jlq_rep);

    strcpy(original_line,line);
       /* get the first token */
    token = strtok(line, s);
    sscanf(token,"%s %s %d",temp,bc,&arg);
    int basic_check=check_basic_command(bc);
    update_node_data(&nd,basic_check,arg);
    if (nd.cmd_flag == REPEAT) //found repeat
    {
        repeat = nd.cmd_arg; //not enqueueing, marking for all upcoming commands
    }
    else {
    enqueue(jlq,nd);
    }
   
   /* walk through other tokens */
   while( token != NULL ) {
        token = strtok(NULL, s);
        if (token){
            sscanf(token,"%s %d",bc,&arg);
            int basic_check=check_basic_command(bc);
            update_node_data(&nd,basic_check,arg);
            if (nd.cmd_flag == REPEAT) //found repeat
            {
                repeat = nd.cmd_arg; //not enqueueing, marking for all upcoming commands
            }
            else
            {
                if (repeat > 0) //if we are in repeat sequence, we enqueue to the repeat jlq instead of the regular one
                {
                    enqueue(jlq_rep, nd);
                }
                else
                {
                    enqueue(jlq, nd);
                }
            }
        }
   }
   
   for (int i = 0; i < repeat; i++) //we copy all jlq_rep nodes to jlq repeat times
   {
        node_data nd;
        node *jlq_rep_next;
        jlq_rep_next = jlq_rep->front;
        while(jlq_rep_next)
        {
            update_node_data(&nd, jlq_rep_next->nd.cmd_flag, jlq_rep_next->nd.cmd_arg);
            enqueue(jlq,nd);
            jlq_rep_next = jlq_rep_next->next;
        }
      
   }
   destruct_queue(jlq_rep);
   

   pthread_mutex_lock(mutex);
   enSharedWorkQueue(swq, jlq, original_line, read_line_time);
   pthread_mutex_unlock(mutex);
}