#include "types.h"
#include "user.h"


int main(void)
{

  int pid;
  int k, n=8;
  int x=0, z;
  int calc_iter=4000000 ;

  

  
  setprio(7); //parent with high priority to print information unintterupted
  for (int j=1 ;j<4 ; j++){  // iterations for larger calculations to consume cpu , to demonstrate the how the priority roundrobin prioritize processes
    printf(1,"-------------------------------------------------------------------------\n");
    printf(1,"Check Priotiy Round Robin with cpu bounded calculation with %d iterations\n",j*calc_iter);
    for ( k = 0; k < n; k++ ) { //parent creates k children
      pid = fork ();
      if ( pid < 0 ) { // fork failiure
        printf(1, "%d failed in fork!\n", getpid());
      } 
      else if (pid > 0) { //parent does nothing and continue to create next child 
        printf(1,"parent %d created child %d\n",getpid(),pid);
        continue;
      }
      else { //child set it's priority by k index, and starts calculation to consume cpu time
          
          setprio(k);
          int begin_ticks=uptime();
          for(z = 0; z < j*calc_iter; z++)
              x = x + 4.25*90.35; //meaningless arithmetic opeartions to consume CPU Time
          int end_ticks=uptime();
          //timer tick occur each 10 ms so we'll calc the time it took each child the caclulate
          printf(1,"(Parent %d) Child %d with priority %d finished calc x= %d after %d [ms]\n",getppid(),getpid(),getprio(),x,(end_ticks-begin_ticks)*10); //print child status
          exit();
          
      }
    }
    for(int i=0;i<n;i++) {// wait for all children to finish
      wait();
    }
    
  }

 
  exit();
}
