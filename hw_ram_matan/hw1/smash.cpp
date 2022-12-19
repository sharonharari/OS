/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include "jobs.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
jobs_table* jobs = new jobs_table();
char lineSize[MAX_LINE_SIZE];
int fg_pid = -1;
char* fg_name = NULL;
bool exists_in_jobs_table = false;
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];

	struct sigaction handle = { 0 };
	handle.sa_handler = &signals_handlers;
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	 sigaction(SIGTSTP,&handle,NULL); //20 -SIGSTP
	 sigaction(SIGINT,&handle,NULL);//2- SIGINT

	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL)
			exit (-1);
	L_Fg_Cmd[0] = '\0';

    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize)-1]='\0';

	 	if(!BgCmd(lineSize, jobs)) continue;
					// built in commands
		ExeCmd(jobs, lineSize, cmdString);

		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

