#ifndef PROCESSINFO_H
#define PROCESSINFO_H


struct processInfo 
{
    int state;         //process state
    int ppid;          //parent PID
    uint sz;           //size of process memory, in bytes
    int nfd;           //number of open file descriptors in the process
    int nrswitch;      //number of context switches in
};

#endif
