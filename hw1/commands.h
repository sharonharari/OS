#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
//const { FALSE , TRUE } bool;
enum job_state {
	Stopped ,
	Running
};
class job{
public:
	pid_t pid;
	std::string cmd;
	job_state state;
	time_t entered_time; // the time the job entered the list
	job(pid_t pida,std::string cmda,job_state statea,time_t entered_timea);
	~job();
};
//int ExeComp(char* lineSize);
int BgCmd(std::map<int, job, less<int>>& mp, std::string args[MAX_ARG], int num_args, std::string cmdString);
int ExeCmd(void* jobs, std::string args[MAX_ARG], int num_args, std::string cmdString);
void ExeExternal(std::string args[MAX_ARG], int num_args, std::string cmdString);


#endif

