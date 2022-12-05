// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
extern pid_t fg_pid; //PID of the foreground process. Initialy/not in use, has value of impossible pid.
extern std::string fg_cmd;



void ctrl_c_handler(int sig_num) {
	std::cout << "smash: caught ctrl-C" << std::endl;
	if (is_fg_exists()) {
		if (!kill(fg_pid, SIGKILL)) { //SIGKILL
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

void ctrl_z_handler(int sig_num) {
	std::cout << "smash: caught ctrl-Z" << std::endl;
	if (is_fg_exists()) {
		std::cout << "FG JOB ID!: " << fg_job_id << std::endl;
		if (addNewJob(fg_pid, fg_cmd, Stopped,fg_job_id)) {
			if (!kill(fg_pid, SIGSTOP)) { //SIGSTOP
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
