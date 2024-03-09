#include "types.h"
#include "user.h"

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
static const char * const procstate_names[] = { //padded names to match ps column width
	[UNUSED] = "UNUSED  ",
	[EMBRYO] = "EMBRYO  ",
	[SLEEPING] = "SLEEPING",
	[RUNNABLE] = "RUNNABLE",
  [RUNNING] = "RUNNING ",
  [ZOMBIE] = "ZOMBIE  "
};

static const int MAX_NUM_CHARS_IN_PRINTED_NUM = 10; //based on maximal value of int/uint

//counts the number of chars for printing an integer
int get_num_chars(int num)
{
  uint num_chars = 0;

  while(num != 0)
  {
    num /= 10;
    ++num_chars;
  }

  return num_chars;
}

//prints spaces by the given amounts
void print_num_spaces(uint num_spaces)
{
  for (int i = 0; i < (num_spaces); i++) 
  {
    printf(1," ");
  }
}

int main(void)
{
  int num_of_procs, maxpid;
  num_of_procs = getNumProc();
  maxpid = getMaxPid();
  printf(1,"Total number of active processes: %d\n",num_of_procs);
  printf(1,"Maximum PID: %d\n",maxpid);
  printf(1,"\n");
  printf(1,"PID       \tSTATE    \tPPID      \tSZ        \tNFD       \tNRSWITCH\n");
  for(int i=0; i <= maxpid; i++){
    struct processInfo pi;
    if(getProcInfo(i,&pi)<0) { //get the process info of the process with PID==i (if exists)
      continue;
    }
    else {
      if (pi.state == UNUSED) continue; //printing only active processes
      //print PID
      printf(1,"%d",i);
      //PID padding
      int num_chars_pid = get_num_chars(i);
      print_num_spaces(MAX_NUM_CHARS_IN_PRINTED_NUM-num_chars_pid);

      //print process state, already padded
      printf(1,"\t%s",procstate_names[pi.state]);
      
      //print parent PID
      int ppid = 0; //the parent PID of the init process (PID=1) is set to 0 by convention
      if (i==0) 
      {
        printf(1,"\t-"); //no parent for PID=0
      }
      else 
      {
        if (i > 1) {
	 ppid = pi.ppid;
	}
        printf(1,"\t%d",ppid); 
      }
      //parent PID padding
      int num_chars_ppid = get_num_chars(ppid);
      print_num_spaces(MAX_NUM_CHARS_IN_PRINTED_NUM - num_chars_ppid);

      //print size
      uint size = pi.sz;
      printf(1,"\t%d",size);
      //size padding
      int num_chars_size = get_num_chars(size);
      print_num_spaces(MAX_NUM_CHARS_IN_PRINTED_NUM - num_chars_size);

      //print NFD
      printf(1,"\t%d",pi.nfd);
      //NFD padding
      int num_chars_nfd = get_num_chars(pi.nfd);
      print_num_spaces(MAX_NUM_CHARS_IN_PRINTED_NUM - num_chars_nfd);

      //print number of context switches - no padding, last field
      printf(1,"\t%d",pi.nrswitch);
      printf(1,"%s\n","");      
    }
  }
  exit();
}
