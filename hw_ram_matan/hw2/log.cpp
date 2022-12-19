#include "log.h"
/*Log file construcor */
log_file::log_file()
{
    std_out_log.open("log.txt",fstream::out);
    pthread_mutex_init(&log_write_mutex, NULL);
}
/* Log file destructor */
log_file::~log_file()
{
    std_out_log.close();
    pthread_mutex_destroy(&log_write_mutex);
}
void log_file::log_add_new_account(int new_account_id,int new_account_password,int new_init_balance,int ATM_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << ATM_id << ": New account id is " << new_account_id << " with password " << new_account_password<< " and initial balance " << new_init_balance << endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::log_delete_account(int id_to_delete,int deleted_balance,int ATM_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << ATM_id << ": Account " << id_to_delete << " is now closed. Balance was "  << deleted_balance << ::endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::log_calc_tax(int random_percent, int tax_amount, int account_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << "Bank: commissions of " << random_percent << " % were charged, the bank gained "<< tax_amount << " $ from account " << account_id << endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::log_withdraw_amount(int account_id,int withdraw_amount,int new_balance,int ATM_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << ATM_id << ": Account " << account_id << " new balance is "<< new_balance << " after " << withdraw_amount << " $ was withdrew" << endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::log_deposit_amount(int account_id,int deposit_amount,int new_balance,int ATM_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << ATM_id << ": Account " << account_id << " new balance is " << new_balance << " after " << deposit_amount << " $ was deposited" << endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::log_transfer_amount(int source_account_id,int target_account_id,int tranfser_amount,int source_account_balance,int target_account_balance,int ATM_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << ATM_id << ": Transfer " << tranfser_amount << " from account " << source_account_id
        << " to account " << target_account_id << " new account balance is " << source_account_balance << " new target account balance is " << target_account_balance << endl;
        pthread_mutex_unlock(&log_write_mutex);
}


void log_file::log_get_account_balance(int account_id,int account_balance,int ATM_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << ATM_id << ": Account " << account_id << " balance is " << account_balance << endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::account_doesnt_exist_error(int ATM_id,int account_id)
{
        pthread_mutex_lock(&log_write_mutex);
        std_out_log << "Error " << ATM_id << ": Your transaction failed - account id "<< account_id << " does not exist" << endl;
        pthread_mutex_unlock(&log_write_mutex);
}

void log_file::account_exist_error(int ATM_id)
{
    pthread_mutex_lock(&log_write_mutex);
    std_out_log << "Error " << ATM_id << ": Your transaction failed - account with the same id exists" << endl;
    pthread_mutex_unlock(&log_write_mutex);
}

void log_file::account_wrong_password_error(int ATM_id,int account_id)
{
    pthread_mutex_lock(&log_write_mutex);
    std_out_log << "Error " << ATM_id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
    pthread_mutex_unlock(&log_write_mutex);
}

void log_file::account_not_enough_balance_error(int ATM_id,int account_id,int required_amount)
{
    pthread_mutex_lock(&log_write_mutex);
    std_out_log << "Error " << ATM_id << ": Your transaction failed – account id " << account_id << " balance is lower than "<< required_amount << endl;
    pthread_mutex_unlock(&log_write_mutex);
}