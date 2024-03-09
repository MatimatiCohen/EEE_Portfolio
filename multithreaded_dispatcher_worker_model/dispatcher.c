#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "commands.h"
#include "shared_work_queue.h"
#include <limits.h>


//Configuration Values
#define NUM_THREADS_MAX 4096
#define NUM_COUNTERS_MAX 100
#define LOG_ENABLED 1

//Basic Commands Values
#define MSLEEP 1
#define INCREMENT 2
#define DECREMENT 3
#define REPEAT 4


//Global variables for dispathcer and worker threads

// mutexes and conds initialize

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t files_mutex[NUM_COUNTERS_MAX];
pthread_mutex_t stats = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//values for stats 
clock_t hw2_start_time;
long long int sum_turnaround_time = 0;
long long int min_turnaround_time = LLONG_MAX;
long long int max_turnaround_time = 0;

// shared work queue 
SharedWorkQueue *swq; 
//Holds user arguments
UserArgs user_args;


//boolean value to exit threads at end of dispatcher execution
bool main_finished_parsing;



//data for threads
struct thread_data_s
{
	int id;
	char *counter_file_name;
	int num_of_threads;
};

//threads data array
struct thread_data_s thread_data[NUM_THREADS_MAX];


// Thread routine

void *thread_worker_func(void *arg)
{
	struct thread_data_s *td = (struct thread_data_s *)arg;
	int id;
	
	id = td->id;

	int num_of_threads = td->num_of_threads;
	char counter_file_name[13];
	char thread_file_name[13];
	node* n;
	node_data nd;
	FILE *counter_fp = NULL, *thread_fp = NULL;
	time_t job_start_time, job_end_time;
	time_t job_start_time_rel = 0, turnaround_time = 0, job_end_time_rel = 0;

	while (1)
	{
		long long int num;
		SWQNode *swq_front;
		pthread_mutex_lock(&mutex);
		// While there's no jobline inside the shared work queue, threads wait on cond
		while (!(swq->front))
		{ 
			//if on top of that the dispatcher finsihed, thread exits
			if (main_finished_parsing)
			{
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL); 
			}
			pthread_cond_wait(&cond, &mutex);
		}
		//save the first value in swq and then dequeue it , so the next thread would be able to take the next line
		swq_front = deSharedWorkQueue(swq);
		pthread_mutex_unlock(&mutex);

		//stats & logging
		job_start_time = time(NULL);
		if (user_args.log_enabled){
			create_thread_file_name(thread_file_name, id);
			thread_fp = fopen(thread_file_name, "a");
			assert(thread_fp != NULL);
			job_start_time_rel = (job_start_time - hw2_start_time)*1000;
			assert(fprintf(thread_fp, "TIME %lld: START job %s\n", (long long int)job_start_time_rel, swq_front->cmd_line)>=0); //(long long int)
			fclose(thread_fp);
		}
	

		//  take the first in line job and execute all the basic commands by order
		while (swq_front->jlq.count > 0) // as long as the worker line still has basic commands to do
		{ 

			n = dequeue(&swq_front->jlq);
			nd = n->nd;

			// execute basic command by the correct flag
			switch (nd.cmd_flag)
			{
			case MSLEEP:
				usleep(nd.cmd_arg * 1000);
				break;
			case INCREMENT:
				create_counter_file_name(counter_file_name, nd.cmd_arg);
				pthread_mutex_lock(&files_mutex[nd.cmd_arg]); //lock file access
				counter_fp = fopen(counter_file_name, "r");
				assert(counter_fp != NULL);
				assert(fscanf(counter_fp, "%lld\n", &num)>=0);
				fclose(counter_fp);
				num += 1;
				counter_fp = fopen(counter_file_name, "w");
				assert(counter_fp != NULL);
				assert(fprintf(counter_fp, "%lld\n", num)>=0);
				fclose(counter_fp);
				pthread_mutex_unlock(&files_mutex[nd.cmd_arg]); //release file access
				break;
			case DECREMENT:
				create_counter_file_name(counter_file_name, nd.cmd_arg);
				pthread_mutex_lock(&files_mutex[nd.cmd_arg]); //lock file access
				counter_fp = fopen(counter_file_name, "r");
				assert(counter_fp != NULL);
				assert(fscanf(counter_fp, "%lld\n", &num)>=0);
				fclose(counter_fp);
				num -= 1;
				counter_fp = fopen(counter_file_name, "w");
				assert(counter_fp != NULL);
				assert(fprintf(counter_fp, "%lld\n", num)>=0);
				fclose(counter_fp);
				pthread_mutex_unlock(&files_mutex[nd.cmd_arg]); //release file access
				break;
			default:
				exit(1); //invalid basic command
				break; 
			}
		}

		//stats & logging
		job_end_time = time(NULL);
		turnaround_time = (job_end_time-swq_front->read_line_time)*1000;
		pthread_mutex_lock(&stats);
		sum_turnaround_time += turnaround_time;
		if (turnaround_time < min_turnaround_time)
		{
			min_turnaround_time = turnaround_time;
		}
		if (turnaround_time > max_turnaround_time)
		{
			max_turnaround_time = turnaround_time;
		}
		pthread_mutex_unlock(&stats);

		if (user_args.log_enabled){
			create_thread_file_name(thread_file_name, id);
			thread_fp = fopen(thread_file_name, "a");
			assert(thread_fp != NULL);
			job_end_time_rel = (job_end_time - hw2_start_time)*1000;
			assert(fprintf(thread_fp, "TIME %lld: END job %s\n", (long long int)job_end_time_rel, swq_front->cmd_line)>=0);
			fclose(thread_fp);
		}

		//free saved swq item
		free(swq_front);
		free(n);
		// the thread should wake all other threads and dispatcher when finish a job line
		pthread_cond_broadcast(&cond); 
	}
}



int main(int argc, char **argv)
{
	hw2_start_time = time(NULL);
	main_finished_parsing = false;
	char dispatcher_file_name[] = "dispatcher.txt";
	int worker_jobs_num = 0;
	FILE *dispatcher_fp = NULL, *stats = NULL;
	time_t end_of_running_time;
	time_t total_running_time = 0, avg_turnaround_time = 0;

	/////////////////////////////
	// Dispatcher Initalize://///

	// create a shared work queue;
	swq = createSharedWorkQueue();

	// read arguments
	DispatcherCommands dc;

	if (!GetArgs(&user_args, argc, argv))
	{
		exit(1);
	}

	if (user_args.log_enabled)
	{
		create_file(dispatcher_file_name, false);
	}

	// create counter file by user arg
	CreateCountersFile(argv, files_mutex);

	//Initialize Threads
	pthread_t tid[NUM_THREADS_MAX]; 
	pthread_attr_t attr;
	int i;

	pthread_attr_init(&attr);

	// threads creation

	for (i = 0; i < user_args.threads_num; i++)
	{
		
		thread_data[i].num_of_threads = user_args.threads_num;
		thread_data[i].id = i;
		if (user_args.log_enabled)
		{
			create_thread_file(i);
		}
		pthread_create(&tid[i], &attr, thread_worker_func, (void *)&thread_data[i]);
	}

	/////////////////////////////////////////////
	///// Read from cmdfile.txt routine:  ///////
	DispatcherCommandInit(&dc);
	FILE *fp = NULL;
	fp = fopen(argv[1], "r");
	assert(fp != NULL);
	size_t buf = 0;
	char *line = NULL;
	time_t read_line_time;
	long long int read_line_time_rel;

	do
	{ // Loop on cmdfile.txt to read/executec ommands
		getline(&line, &buf, fp);
		
		//Dipatcher Log:
		read_line_time = time(NULL);
		if (user_args.log_enabled){
			if(strlen(line)>6){
				dispatcher_fp = fopen(dispatcher_file_name, "a");
				assert(dispatcher_fp != NULL);
				read_line_time_rel = (read_line_time - hw2_start_time)*1000;
				assert(fprintf(dispatcher_fp, "TIME %lld: read cmd line: %s", (long long int)read_line_time_rel, line)>=0);
				fclose(dispatcher_fp);
			}
		}
		// Check if line is a dispatcher command
		DispatcherCommandUpdate(&dc, line);
		if (DispatcherCmdExecuter(&dc, &user_args, tid) == 2)
		{
			// wait till shared queue is empty and then continue
			pthread_mutex_lock(&mutex);
			while (swq->front)
			{ 
				pthread_cond_wait(&cond, &mutex);
			}
			pthread_mutex_unlock(&mutex);

		}
		// Worker line 
		if (dc.worker_flag)
		{
			worker_jobs_num++;
			// offload a worker line to shared work queue
			create_job_line(line, swq, &mutex, read_line_time);
			// when there's a job line , wake threads
			pthread_cond_broadcast(&cond);

		}

		free(line);
		buf = 0;
		line = NULL;

	} while (!feof(fp));
	fclose(fp);

	// a boolean condition to exit threads and avoid deadlock
	main_finished_parsing = true;

	

	//bye bye beautiful threads (wait for them to finish)
	for (i = 0; i < user_args.threads_num; i++)
	{
		pthread_join(tid[i], NULL);
	}
	
	//end of dispatcher.txt log
	if (user_args.log_enabled){
		dispatcher_fp = fopen(dispatcher_file_name, "a");
		assert(dispatcher_fp != NULL);
		assert(fprintf(dispatcher_fp, "\n")>=0); 
		fclose(dispatcher_fp);	
	}	
    

	//Stats 
	end_of_running_time = time(NULL);
	total_running_time = 1000*(end_of_running_time-hw2_start_time);
	if (worker_jobs_num>0){
		avg_turnaround_time = sum_turnaround_time/worker_jobs_num;
	}
	

	stats = fopen("stats.txt", "w");
	assert(stats != NULL);
	fprintf(stats, "total running time: %lld milliseconds\n", (long long int)total_running_time);
	fprintf(stats, "sum of jobs turnaround time: %lld milliseconds\n", (long long int)sum_turnaround_time);
	fprintf(stats, "min job turnaround time: %lld milliseconds\n",(long long int) min_turnaround_time);
	fprintf(stats, "average job turnaround time: %lld milliseconds\n", (long long int)avg_turnaround_time);
	fprintf(stats, "max job turnaround time: %lld milliseconds\n",(long long int) max_turnaround_time);
	fclose(stats);
	
	//Dispatcher finished
	free(line);
	free(swq);
	exit(0);
}
