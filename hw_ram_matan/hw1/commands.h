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

// ***** my includes *****
#include <iostream>
#include <fstream>
#include <stdbool.h>
//#include <filesystem>
#include <list>
#include "jobs.h"

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

// *****my defines*****
#define MAX_HISTORY_CMDS 50
//extern jobs_table* jobs;

typedef enum { FALSE , TRUE } BOOL;
int BgCmd(char* lineSize, jobs_table* jobs);
int ExeCmd(jobs_table* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);

extern int fg_pid;
extern char* fg_name;
extern bool exists_in_jobs_table; 
extern jobs_table* jobs;
#endif

