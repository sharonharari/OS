/*	smash.cpp
main file. This file contains the main function of smash
*******************************************************************/

#include "commands.h"
#include "signals.h"




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
	ctrl_c.sa_flags = SA_RESTART;
	struct sigaction ctrl_z = { 0 };
	ctrl_z.sa_handler = ctrl_z_handler;
	ctrl_z.sa_flags = SA_RESTART;
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */
	if (sigaction(SIGINT, &ctrl_c, NULL) == -1) {
		perror("smash error: sigaction failed");
	}
	if(sigaction(SIGTSTP, &ctrl_z, NULL) == -1){
		perror("smash error: sigaction failed");
	}
	/************************************/

	/************************************/
    while (1){
    	printf("smash > ");
		update_jobs_list();
		std::string line,cmdString;
		std::getline (std::cin,line);
		cmdString = line;
		std::string args[MAX_ARG];
		int num_args = cmdline_split_into_arguments(line, args);
					// background command	
		if (!is_built_in_cmd(args[CMD])) {
			if (args[CMD]!="&" && args[num_args][args[num_args].length() - 1] == '&') {
				args[num_args].pop_back();
				if (args[num_args].empty()) {
					num_args -= 1;
				}
				if (BgCmd(args, num_args, cmdString)) {
					//ERROR background
				}
			}
			else {
				if (ExeExternal(args, num_args, cmdString)) {
					//ERROR foreground
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
	free(last_path);
	free(current_path);
    return 0;
}

