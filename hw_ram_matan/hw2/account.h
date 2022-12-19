#ifndef  _ACCOUNT_H
#define  _ACCOUNT_H

#include <pthread.h>
#include <string>
#include "log.h"

using std::string;

class account{
    int account_id;
    int account_password;
    int account_balance;
    int num_of_readers;
    pthread_mutex_t account_read_mutex;
    pthread_mutex_t account_write_mutex;
public:
    /*Constructor*/
    account(int requested_id, int requested_password, int requested_init_balance);
    /* Destructor */
    ~account();
    /* Account functions */
    int get_account_id();
    int get_account_password();
    int get_account_balance();
    void account_read_lock();
    void account_read_unlock();
    void account_write_lock();
    void account_write_unlock();
    void withdraw_amount(int withdraw_amount,log_file* out_file,int ATM_id);
    void deposit_amount(int deposit_amount,log_file* out_file,int ATM_id);
    void transfer_amount(account* target_account, int transfer_amount,log_file* out_file,int ATM_id);
    void charge_taxes(int tax_amount , int random_percent, log_file* out_file);
    void print_account_balance(log_file* out_file, int ATM_id);
};

#endif