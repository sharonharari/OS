/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "signals.h"
#include <iostream>
#include <map>


#define MAX_LINE_SIZE 80
#define MAXARGS 20




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
		if (!is_built_in_cmd(args[CMD])) {
			if (args[CMD]!="&" && args[num_args][args[num_args].length() - 1] == '&') {
				args[num_args].pop_back();
				if (args[num_args].empty()) {
					num_args -= 1;
				}
				if (!BgCmd(args, num_args, cmdString)) {
					//ERROR background
					std::cerr << "smash error: BgCmd faild" << std::endl;
				}
			}
			else {
				if (!ExeExternal(args, num_args, cmdString)) {
					//ERROR foreground
					std::cerr << "smash error: ExeExternal faild" << std::endl;
				}
			}

		}
		else {
			if (args[num_args][args[num_args].length() - 1] == '&') {
				args[num_args].pop_back();
				if (args[num_args].empty()) {
					num_args -= 1;
				}
			}
			// built in commands
			ExeCmd(args, num_args, cmdString);
		}
		fg_clear();
	}
    return 0;
}

