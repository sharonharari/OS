#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
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
	int pid;
	std::string cmd;
	job_state state;
	time_t entered_time; // the time the job entered the list
	job(int pida,std::string cmda,job_state statea,time_t entered_timea);
	~job();
};
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs);
int ExeCmd(void* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);


#endif

