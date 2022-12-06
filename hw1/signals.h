#ifndef _SIGS_H
#define _SIGS_H
#include <csignal>
#include "commands.h"

void ctrl_c_handler(int sig_num);
void ctrl_z_handler(int sig_num);

#endif

