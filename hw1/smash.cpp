/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "signals.h"
#include <iostream>
#include <map>
#include <string>
#include <string_view>
using namespace std;

#define MAX_LINE_SIZE 80
#define MAXARGS 20




enum job_state {
	Stopped ,
	Running
};




int last_job;
char * last_path;
std::map<int, job, less<int>> mp;

//FG handling
pid_t fg_pid = -1; //PID of the foreground process. Initialy/not in use, has value of impossible pid.
std::string fg_cmd;

bool is_fg_exists() {
	if ((fg_pid != -1) && (!fg_cmd.empty)) {
		return true;
	}
	//std::cerr << "fg container flawed/broken." << std::endl;
	return false;
}
void fg_clear() {
	fg_pid = -1;
	fg_cmd.clear();
}
void fg_insert(pid_t newPid, std::string newCmd) {
	fg_pid = newPid;
	fg_cmd = newCmd;
}



//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
  

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	struct sigaction ctrl_c = { 0 };
	ctrl_c.sa_handler = ctrl_c_handler;
	struct sigaction ctrl_z = { 0 };
	ctrl_z.sa_handler = ctrl_z_handler;
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */
	sigaction(SIGINT, &ctrl_c, NULL);
	sigaction(SIGTSTP, &ctrl_z, NULL);
	/************************************/

	/************************************/
	// Init globals 



    last_path = NULL;

	

	
    while (1){
	 	std::cout << "smash > ";
		std::string line,cmdString;
		std::getline (std::cin,line);
		cmdString = line;
		std::string args[MAX_ARG];
		int num_args = cmdline_split_into_arguments(line, args);
					// perform a complicated Command
		//if(!ExeComp(lineSize)) continue; 
					// background command	
	 	if(!BgCmd(jobs, args, num_args, cmdString)) continue;
					// built in commands
		ExeCmd(jobs, args, num_args, cmdString);
	}
    return 0;
}

