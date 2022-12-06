#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>    // std::find
#include <cerrno>
#include <cstdio>
#include <iostream>

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

enum job_state {
	Stopped ,
	Running
};

const int CMD = 0;
const int SUCCESS = 0;
const int FAILED = -1;
const std::string BUILT_IN_CMD[] = { "showpid" ,"pwd","cd","jobs","kill","fg","bg","quit","diff" };

extern pid_t fg_pid; //PID of the foreground process. Initialy/not in use, has value of impossible pid.
extern std::string fg_cmd;
extern int fg_job_id;
extern char* last_path;
extern char* current_path;
class job{
public:
	pid_t pid;
	std::string cmd;
	job_state state;
	time_t entered_time; // the time the job entered the list
	job();
	job(pid_t pida,std::string cmda,job_state statea,time_t entered_timea);
	job(const job& new_job);
	job& operator=(const job& new_job);
	~job();
};
//int ExeComp(char* lineSize);
void update_jobs_list();
int BgCmd(std::string args[MAX_ARG], int num_args, std::string cmdString);
int ExeCmd(std::string args[MAX_ARG], int num_args, std::string cmdString);
int ExeExternal(std::string args[MAX_ARG], int num_args, std::string cmdString);
bool is_built_in_cmd(std::string cmd);
bool is_fg_exists();
void fg_clear();
void fg_insert(pid_t newPid, std::string newCmd, int job_id = -1);
int cmdline_split_into_arguments(std::string line, std::string(&args)[MAX_ARG], std::string delimiters = " \t\n");
bool addNewJob(pid_t pID, std::string cmd, job_state state = Running, int job_id = -1);
int arg_in_map(std::string& arg);
bool is_number_char(char* str);
bool is_number(std::string& str);
int find_stopped();
bool is_fg_have_job_id();
#endif

