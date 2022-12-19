#ifndef _JOBS_H
#define _JOBS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <list>
#include <iterator>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <fcntl.h> 

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
using namespace std;

class job{
public:
	int job_index;
	char* job_name;
	int job_pid;
	time_t timer;
	bool job_stopped;
	bool job_finished;
    job(int job_index_r,char* job_name_r,int job_pid_r,bool job_stopped_r,bool job_finished_r);
	void update_job_status();
	void print_current_job();
};

class jobs_table{
public:
	list<job> jobs_list;
	int num_of_jobs;
	jobs_table();
	void insert_new_job(char* job_name_r, int job_pid_r, bool job_stopped_r, bool job_finished_r);
	void print_jobs();
	int get_job_PID(char* string_numb);
	char* get_PID_name(char* string_numb);
	int get_recent_PID();
	char* get_recent_job_name();
	int get_recent_stopped_PID();
	char* get_recent_stopped_name();
	bool get_stopped_status(char* n);
};
#endif