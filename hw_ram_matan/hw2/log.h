#ifndef _LOG_H
#define _LOG_H
#include <fstream>
#include <pthread.h>
#include <iostream>

using namespace std;


class log_file{
    ofstream std_out_log;
    pthread_mutex_t log_write_mutex;
public:
    /*Log constructor*/
    log_file();
    /*Log destructor*/
    ~log_file();

    /* Successful Action Log Functions */
    void log_add_new_account(int new_account_id,int new_account_password,int new_init_balance,int ATM_id);
    void log_calc_tax(int random_percent, int tax_amount, int account_id);
    void log_withdraw_amount(int account_id,int withdraw_amount,int new_balance,int ATM_id);
    void log_deposit_amount(int account_id,int deposit_amount,int new_balance,int ATM_id);
    void log_transfer_amount(int source_account_id,int target_account_id,int tranfser_amount,int source_account_balance,int target_account_balance,int ATM_id);
    void log_get_account_balance(int account_id,int account_balance,int ATM_id);
    void log_delete_account(int id_to_delete,int deleted_balance,int ATM_id);

    /* Failed Actions Log Functions */
    void account_doesnt_exist_error(int ATM_id,int account_id);
    void account_exist_error(int ATM_id);
    void account_wrong_password_error(int ATM_id,int account_id);
    void account_not_enough_balance_error(int ATM_id,int account_id,int required_amount);

};

#endif

