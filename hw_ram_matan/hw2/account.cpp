#include "account.h"
#include <iostream>

/*Constructor */
account::account(int requested_id, int requested_password, int requested_init_balance):account_id(requested_id),account_password(requested_password),account_balance(requested_init_balance),num_of_readers(0)
{
    pthread_mutex_init(&account_read_mutex, NULL);
    pthread_mutex_init(&account_write_mutex, NULL);
}

/*Destructor*/

account::~account()
{
    pthread_mutex_destroy(&account_read_mutex);
    pthread_mutex_destroy(&account_write_mutex);
}

/* Account Locks*/
void account::account_read_lock()
{
    pthread_mutex_lock(&account_read_mutex);
   num_of_readers -= 1;
    if(num_of_readers == 0)
        pthread_mutex_unlock(&account_write_mutex);
    pthread_mutex_unlock(&account_read_mutex);
}

void account::account_read_unlock()
{
    pthread_mutex_lock(&account_read_mutex);
    num_of_readers += 1;
    if(num_of_readers == 1)
        pthread_mutex_lock(&account_write_mutex);
    pthread_mutex_unlock(&account_read_mutex);
}

void account::account_write_lock()
{
  pthread_mutex_lock(&account_write_mutex);
}


void account::account_write_unlock()
{
    pthread_mutex_unlock(&account_write_mutex);
}

/*Account bank functions */
int account::get_account_id()
{
    return account_id;
}

int account::get_account_password()
{
    return account_password;
}

int account::get_account_balance()
{
    account_read_lock();
    int correct_balance = this -> account_balance;
    account_read_unlock();
    return correct_balance;
}

void account::withdraw_amount(int withdraw_amount,log_file* out_file,int ATM_id)
{
    account_write_lock();
    account_balance -=  withdraw_amount;
    out_file->log_withdraw_amount(account_id,withdraw_amount,account_balance,ATM_id);
    account_write_unlock();
}


void account::deposit_amount(int deposit_amount,log_file* out_file,int ATM_id)
{
    account_write_lock();
    account_balance += deposit_amount;
    out_file->log_deposit_amount(account_id,deposit_amount,account_balance,ATM_id);
    account_write_unlock();
}

void account::transfer_amount(account* target_account, int transfer_amount,log_file* out_file,int ATM_id)
{
    /* Ensuring locking the correct order to avoid deadlock */
    if(this->account_id > target_account->account_id)
    {
        this->account_write_lock();
        target_account->account_write_lock();
    }
    else
    {
        target_account->account_write_lock();
        this->account_write_lock();
    }
    this -> account_balance -= transfer_amount;
    target_account->account_balance += transfer_amount;
    out_file->log_transfer_amount(this->account_id, target_account->account_id ,transfer_amount,this->account_balance , target_account ->account_balance,ATM_id);
    target_account->account_write_unlock();
    this -> account_write_unlock();
}

void account::charge_taxes(int tax_amount , int random_percent, log_file* out_file)
{
    account_write_lock();
    account_balance -= tax_amount;
    out_file->log_calc_tax(random_percent,tax_amount,account_id);
    account_write_unlock();
}

void account::print_account_balance(log_file* out_file, int ATM_id)
{
    account_read_lock();
    out_file->log_get_account_balance(account_id,account_balance,ATM_id);
    account_read_unlock();
}