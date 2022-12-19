#ifndef _ATMS_H
#define _ATMS_H
#include <pthread.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "bank.h"

using namespace std;

class ATM{
public:
    int ATM_id;
    string ATM_file_name;
    bank* our_Bank;
    ATM();
    ATM(int input_atm_id, string input_atm_file ,bank* input_bank);
    void run_atm_commands();
};


#endif
