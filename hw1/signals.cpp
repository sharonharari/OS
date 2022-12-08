// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
extern pid_t fg_pid; //PID of the foreground process. Initialy/not in use, has value of impossible pid.
extern std::string fg_cmd;


/*
	ctrl_c_handler:
		A SIGINT signal handler. By given ctrl-c, the fg process is being killed.
*/
void ctrl_c_handler(int sig_num) {
	std::cout << "smash: caught ctrl-C" << std::endl;
	if (is_fg_exists()) {
		if (!kill(fg_pid, SIGKILL)) {
			std::cout << "smash: process " << fg_pid << " was killed" << std::endl;
			fg_clear();
		}
		else {
			std::perror("smash error: kill failed");
		}
	}
	else {
		printf("smash > ");
		fflush(stdout);

	}
}


/*
	ctrl_z_handler:
		A SIGTSTP signal handler. By given ctrl-z, the fg process is stopped, turned to a job and added to the jobs ADT.
*/
void ctrl_z_handler(int sig_num) {
	std::cout << "smash: caught ctrl-Z" << std::endl;
	if (is_fg_exists()) {
		if (addNewJob(fg_pid, fg_cmd, Stopped,fg_job_id)) {
			if (!kill(fg_pid, SIGSTOP)) {
				std::cout << "smash: process " << fg_pid << " was stopped" << std::endl;
				fg_clear();
			}
			else {
				std::perror("smash error: kill failed");
			}
		}
		else {
			std::cerr << "smash error: addNewJob failed\n";
		}
	}
	else {
		printf("smash > ");
		fflush(stdout);
	}
}
