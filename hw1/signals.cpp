// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"


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
}

void ctrl_z_handler(int sig_num) {
	std::cout << "smash: caught ctrl-Z" << std::endl;
	if (is_fg_exists()) {
		if (addNewJob(fg_pid, fg_cmd, Stopped)) {
			if (!kill(fg_pid, SIGSTOP)) {
				std::cout << "smash: process " << fg_pid << " was stopped" << std::endl;
				fg_clear();
			}
			else {
				std::perror("smash error: kill failed");
			}
		}
		else {
			/*
				addNewJob() faliure handling.
			*/
		}
	}
}
