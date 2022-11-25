// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

void process_sigint_handler(int sig_num) {
	std::cout << "smash: caught ctrl-C" << std::endl;
	if (fg_pid != -1) {
		if (!kill(fg_pid, SIGKILL)) {
			std::cout << "smash: process " << fg_pid << " was killed" << std::endl;
		}
		else {
			std::cerr << "failed kill over SIGKILL on pid:" << fg_pid << std::endl;
		}
	}
}

void process_sigtstp_handler(int sig_num) {
	std::cout << "smash: caught ctrl-Z" << std::endl;
	if (fg_pid != -1) {
		if (addNewJob()) {
			if (!kill(fg_pid, SIGSTOP)) {
				std::cout << "smash: process " << fg_pid << " was stopped" << std::endl;
			}
			else {
				std::cerr << "failed kill over SIGSTOP on pid:" << fg_pid << std::endl;
			}
		}
		/*
			addNewJob() faliure handling.
		*/
	}
}