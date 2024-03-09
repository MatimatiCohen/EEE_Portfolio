#include "commands.h"

#include <unistd.h>

#define MSLEEP 1
#define INCREMENT 2
#define DECREMENT 3
#define REPEAT 4

void update_node_data(node_data* nd, int basic_check, int arg );
int check_basic_command(char *bc);


void DispatcherCommandInit(DispatcherCommands *dc){
    dc->cmd_val=0;
    dc->cmd=NULL;
    dc->wait_flag=0;
    dc->worker_flag=0;
}

void DispatcherCommandUpdate(DispatcherCommands *dc,char* cmd_line){
    char *temp;
    temp=strtok(cmd_line," ");
    if (is_worker_cmd(temp)){
        dc->worker_flag=1;
        return;
    }
    if (temp){
     dc->cmd=temp;
    }
    else {
        dc->cmd=cmd_line;
    }

    temp=strtok(NULL," ");
    if (temp)
    dc->cmd_val=atoi(temp);
    dc->worker_flag=0;

}

bool is_worker_cmd(char* string){
    int i;
    char w[]="worker";
    for (i=0;i<6;i++){
        if (string[i]!=w[i]){
            return false;
        }
    }
    string[i]=' '; // brings back the original line after strtok so it can be parsed
    return true;

}

int DispatcherCmdExecuter(DispatcherCommands *dc,UserArgs *user_args,pthread_t *tid){
    if (dc->cmd==NULL) return 0;
    if (!strcmp(dc->cmd,"dispatcher_msleep")){
        usleep(1000*dc->cmd_val);
        return 1;
    }
    else if (!strcmp(TruncateLastChar(dc->cmd),"dispatcher_wait")&&(!dc->wait_flag)){
        dc->wait_flag=1;
        return 2;
    }
    return 0;
}



// queue functions:
void initialize(JobLineQueue *q)
{
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
}

int isempty(JobLineQueue  *q)
{
    return (q->rear == NULL);
}


void enqueue(JobLineQueue *q, node_data nd)
{
    node *tmp;
    tmp = malloc(sizeof(node));
    tmp->nd=nd;
    tmp->next = NULL;
    if(!isempty(q))
    {
        q->rear->next = tmp;
        q->rear = tmp;
    }
    else
    {
        q->front = q->rear = tmp;
    }
    q->count++;
}

node* dequeue(JobLineQueue *q)
{
    node *tmp;
    tmp = q->front;
    q->front = q->front->next;
    q->count--;
    return(tmp);
}



void init_node_nd(node_data *nd){
    nd->cmd_arg=0;
    nd->cmd_flag=0;
}


void update_node_data(node_data* nd, int basic_check, int arg ){
    if (basic_check>0){
        nd->cmd_arg=arg;
        nd->cmd_flag=basic_check;
    }
    
}

int check_basic_command(char *bc){
    if (!strcmp(bc,"msleep")){
        return MSLEEP;
    }
    if (!strcmp(bc,"increment")){
        return INCREMENT;
    }
    if (!strcmp(bc,"decrement")){
        return DECREMENT;
    }
    if (!strcmp(bc,"repeat")){
        return REPEAT;
    }
    return 0;
}


void destruct_queue(JobLineQueue* q) {
    node *tmp;
    while (q->count) {
        tmp = dequeue(q);
        free(tmp);
    }
    free(q);
}
