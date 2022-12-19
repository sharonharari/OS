#include <pthread.h>
#include "ATMs.h"
#include "bank.h"
#include "log.h"

bool finished_threads = false;
using namespace std;

void* run_cmd(void* atm){
    ATM* tmpATM = (ATM*)atm;
    tmpATM->run_atm_commands();
    pthread_exit(NULL);
    return NULL;
}

void* tax_charges_thread(void* args){
    bank* our_bank = (bank*)args;
    our_bank->bank_tax_charges();
    pthread_exit(NULL);
    return NULL;
}

void* print_accounts_status_thread(void* args){
    bank* our_bank = (bank*)args;
    our_bank->bank_accounts_status();
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char* argv[]) {
    int atm_num = atoi(argv[1]);
    if (!atm_num || (argc-2 != atm_num) )
    {
		cerr << "atoi failed : Number of ATMS is not a valid value " << endl;
        return 1;
    }
    log_file log_file;
    bank* our_bank = new bank(&log_file);
    pthread_t tax_thread;
    pthread_t bank_status_thread;

    int check_1 = pthread_create(&tax_thread,NULL,tax_charges_thread,our_bank);
    int check_2 = pthread_create(&bank_status_thread,NULL,print_accounts_status_thread,our_bank);
    if((check_1 != 0) || (check_2 != 0))
    {
        cerr << "pthread_create failed" << endl;
        exit(1);
    }
    ATM* atm = new ATM[atm_num];
    for(int i=0; i<atm_num; i++)
        atm[i] = ATM(i,argv[i+2],our_bank);

    pthread_t* threads = new pthread_t[atm_num];
    for(int i=0;i<atm_num;i++)
    {
        if(pthread_create(&threads[i], NULL, run_cmd, &atm[i]) != 0)
        {
    		cerr << "atM_pthread_create failed " << endl;
    		exit(1);           
        }
    }
    for(int i=0;i<atm_num;++i)
        if(pthread_join(threads[i], NULL) != 0)
        {
            cerr << "pthread_join failed" << endl;
            exit(1);
        }

    finished_threads = true;
    int check_3 = pthread_join(tax_thread, NULL);
    int check_4 = pthread_join(bank_status_thread, NULL);
    if((check_3 != 0) || (check_4 != 0))
    {
        cerr << "pthread_join failed" << endl;
        exit(1);
    }
    delete[] threads;
    return 0;
}



