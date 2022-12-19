#ifndef _BANK_H
#define _BANK_H
#include "account.h"
#include "log.h"
#include <pthread.h>
#include <iostream>
#include <map>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

class bank {
    map <int,account*> accounts_map;
    int bank_total_charges;
    int num_of_bank_readers;
    log_file* main_log_file;
    pthread_mutex_t bank_read_mutex;
    pthread_mutex_t bank_write_mutex;

public:
    bank(log_file* main_log_file);
    ~bank();
    /*Locks functions */
    void bank_read_lock();
    void bank_read_unlock();
    void bank_write_lock();
    void bank_write_unlock();

    /*Bank functions */
    void bank_tax_charges();
    void bank_accounts_status();
    void bank_add_new_account(int new_account_id,int new_account_password,int new_init_balance,int ATM_id);
    void bank_transfer_amount(int source_account_id,int source_account_password,int target_account_id,int transfer_amount,int ATM_id);
    void bank_withdraw_amount(int withdraw_account_id,int withdraw_account_password,int withdraw_amount,int ATM_id);
    void bank_deposit_amount(int deposit_account_id,int deposit_account_password,int deposit_amount,int ATM_id);
    void bank_check_account_balance(int chk_balance_account_id,int chk_balance_account_password,int ATM_id);
    void bank_delete_account(int id_to_delete,int password_to_delete,int ATM_id);
    
};


#endif