#include "bank.h"
extern bool finished_threads;

/* Global iterators for for each bank function */

map<int, account*>::iterator accounts_iter ;
map<int, account*>::iterator transfer_source_iter ;
map<int, account*>::iterator transfer_target_iter ;
map<int, account*>::iterator account_withdraw_iter ;
map<int, account*>::iterator account_deposit_iter;
map<int, account*>::iterator account_balance_iter;
map<int, account*>::iterator account_print_iter;
map<int, account*>::iterator account_delete_iter;


/*Bank constructor */
bank::bank(log_file* given_log_file):bank_total_charges(0),num_of_bank_readers(0),main_log_file(given_log_file)
{
    /*Creating map of accounts_map */
    accounts_map = map <int,account*> ();
    pthread_mutex_init(&bank_read_mutex, NULL);
    pthread_mutex_init(&bank_write_mutex, NULL);
}
/*Bank Destructor */

bank::~bank()
{
    pthread_mutex_destroy(&bank_read_mutex);
    pthread_mutex_destroy(&bank_write_mutex);
}


void bank::bank_read_lock()
{
    pthread_mutex_lock(&bank_read_mutex);
    num_of_bank_readers += 1;
    if(num_of_bank_readers ==1)
        pthread_mutex_lock(&bank_write_mutex);
    pthread_mutex_unlock(&bank_read_mutex);
}

void bank::bank_read_unlock()
{
    pthread_mutex_lock(&bank_read_mutex);
    num_of_bank_readers -= 1;
    if(num_of_bank_readers == 0)
        pthread_mutex_unlock(&bank_write_mutex);
    pthread_mutex_unlock(&bank_read_mutex);
}

void bank::bank_write_lock()
{
    pthread_mutex_lock(&bank_write_mutex);
}

void bank::bank_write_unlock()
{
    pthread_mutex_unlock(&bank_write_mutex);
}

void bank::bank_tax_charges()
{
    
    while (!finished_threads)
    {
        double random_tax_percent = double(((rand() % 3) + 2)) / 100;
        bank_read_lock();
        for (accounts_iter = accounts_map.begin(); accounts_iter != accounts_map.end(); accounts_iter++)
        {
            
            /*Obtaining the account from the map */
            account* account_to_charge = accounts_iter -> second;
            int amount_to_charge = int(account_to_charge->get_account_balance() * random_tax_percent);
            account_to_charge->charge_taxes(amount_to_charge,(random_tax_percent * 100),main_log_file);
            bank_total_charges += amount_to_charge;
        }
        bank_read_unlock();
        sleep(3);
    }
}



void bank::bank_accounts_status()
{
    while(!finished_threads)
    {
        bank_read_lock();
        cout << "\033[2J" << "\033[1;1H";
        cout << "Current Bank Status\n";
        for(account_print_iter = accounts_map.begin(); account_print_iter!=accounts_map.end(); account_print_iter++)
        {
            account* account_to_print = account_print_iter->second;
            cout << "Account " << account_to_print -> get_account_id() << ": Balance - " << account_to_print-> get_account_balance() << " $ , Account Password - " << account_to_print->get_account_password() << "\n";
        }
        cout << "The Bank has " << bank_total_charges << " $" << endl;
        bank_read_unlock();
        usleep(500000);
    }
}

void bank::bank_add_new_account(int new_account_id,int new_account_password,int new_init_balance,int ATM_id)
{
    bank_write_lock();
    account* new_account = new account(new_account_id,new_account_password,new_init_balance);
    /* Adding the new account to the list , if map.insert returns 0 then the specific key exists */
    if(accounts_map.insert(pair<int,account*>(new_account_id,new_account)).second == 0)
    {
        main_log_file->account_exist_error(ATM_id);
        bank_write_unlock();
        sleep(1);
        return;
    }
    else 
    {
        /*Key(ID)  doesnt exist we can add the new account */
        main_log_file->log_add_new_account(new_account_id,new_account_password,new_init_balance,ATM_id);
    }
    sleep(1);
    bank_write_unlock();
}

void bank::bank_delete_account(int id_to_delete,int password_to_delete,int ATM_id)
{
    bank_read_lock();
    account_delete_iter = accounts_map.find(id_to_delete);
    if(account_delete_iter==accounts_map.end()){
        main_log_file->account_doesnt_exist_error(ATM_id,id_to_delete);
        bank_read_unlock();
        sleep(1);
        return;
    }
    account* account_to_delete = account_delete_iter->second;
    if(account_to_delete-> get_account_password() != password_to_delete){
        main_log_file->account_wrong_password_error(ATM_id,id_to_delete);
        bank_read_unlock();
        sleep(1);
        return;
    }
    /* Now need to delete the account */
    int deleted_balance = account_to_delete -> get_account_balance();
    /*Delete failed */
    if(accounts_map.erase(id_to_delete) == 0){
        main_log_file->account_doesnt_exist_error(ATM_id,id_to_delete);
        bank_read_unlock();
        sleep(1);
        return;
    }
    else{
        main_log_file->log_delete_account(id_to_delete,deleted_balance,ATM_id);
    }
    sleep(1);
    bank_read_unlock();
}

void bank::bank_transfer_amount(int source_account_id,int source_account_password,int target_account_id,int transfer_amount,int ATM_id)
{
    bank_read_lock();
    transfer_source_iter = accounts_map.find(source_account_id);
    /* Cant find the source account case */
    if(transfer_source_iter == accounts_map.end())
    {
        main_log_file->account_doesnt_exist_error(ATM_id,source_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
    transfer_target_iter = accounts_map.find(target_account_id);
     /* Cant find the target account case */
    if(transfer_target_iter == accounts_map.end())
    {
        main_log_file->account_doesnt_exist_error(ATM_id,target_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }

    account* source_account= transfer_source_iter->second;
    account* target_account = transfer_target_iter->second;
  

    if(source_account-> get_account_id() == target_account-> get_account_id()){
        main_log_file->account_doesnt_exist_error(ATM_id,source_account->get_account_id());
        bank_read_unlock();
        sleep(1);
        return;
    }

    if(source_account->get_account_password() != source_account_password)
    {
        main_log_file-> account_wrong_password_error(ATM_id,source_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
 
    if(source_account->get_account_balance() < transfer_amount)
    {
        main_log_file-> account_not_enough_balance_error(ATM_id,source_account_id,transfer_amount);
        bank_read_unlock();
        sleep(1);
        return;
    }
    /*If got here , we can do the transaction */
    source_account->transfer_amount(target_account,transfer_amount,main_log_file,ATM_id);
    sleep(1);
    bank_read_unlock();
}

void bank::bank_withdraw_amount(int withdraw_account_id,int withdraw_account_password,int withdraw_amount,int ATM_id)
{
    bank_read_lock();
    account_withdraw_iter = accounts_map.find(withdraw_account_id);
    if(account_withdraw_iter == accounts_map.end())
    {
        main_log_file->account_doesnt_exist_error(ATM_id,withdraw_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
    account* account_to_withdraw = account_withdraw_iter->second;
    if(account_to_withdraw->get_account_password() != withdraw_account_password)
    {
        main_log_file->account_wrong_password_error(ATM_id,withdraw_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }

    
    if(account_to_withdraw->get_account_balance() < withdraw_amount)
    {
        main_log_file -> account_not_enough_balance_error(ATM_id,account_to_withdraw->get_account_id(),withdraw_amount);
        bank_read_unlock();
        sleep(1);
        return;
    }
    /*if were here withdraw is good */
    account_to_withdraw -> withdraw_amount(withdraw_amount,main_log_file,ATM_id);
    sleep(1);
    bank_read_unlock();
}

void bank::bank_deposit_amount(int deposit_account_id,int deposit_account_password,int deposit_amount,int ATM_id)
{
    bank_read_lock();
    account_deposit_iter = accounts_map.find(deposit_account_id);
    if (account_deposit_iter == accounts_map.end()) {
    main_log_file->account_doesnt_exist_error(ATM_id,deposit_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
    account* account_to_deposit = account_deposit_iter->second;
    if(account_to_deposit-> get_account_password() != deposit_account_password){
        main_log_file->account_wrong_password_error(ATM_id,deposit_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
    account_to_deposit->deposit_amount(deposit_amount,main_log_file,ATM_id);
    sleep(1);
    bank_read_unlock();
}


void bank::bank_check_account_balance(int chk_balance_account_id,int chk_balance_account_password,int ATM_id)
{
    bank_read_lock();
    account_balance_iter = accounts_map.find(chk_balance_account_id);
    if(account_balance_iter==accounts_map.end()){
        main_log_file->account_doesnt_exist_error(ATM_id,chk_balance_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
    account* check_balance_account = account_balance_iter->second;
    if(check_balance_account-> get_account_password() != chk_balance_account_password){
        main_log_file->account_wrong_password_error(ATM_id,chk_balance_account_id);
        bank_read_unlock();
        sleep(1);
        return;
    }
    check_balance_account -> print_account_balance(main_log_file,ATM_id);
    sleep(1);
    bank_read_unlock();
}


