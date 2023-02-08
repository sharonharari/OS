#include "Bank.h"

std::vector<std::string> split(std::string const& str, const char delim)
{
	std::istringstream split(str);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, delim); tokens.push_back(each));
	return tokens;
}
int random_percentage() {
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(1, 5);
	return distrib(gen);
}

/*
*	Bank ADT definition
*/
bool Bank::passwordCompare(int account_id, std::string password) {
	return (this->mp_ac[account_id].getPassword() == password);
}
bool Bank::isAccountExist(int account_id) {
	return (!(this->mp_ac.find(account_id) == this->mp_ac.end()));
}
Bank::Bank() :profit(INITIAL_BANK_PROFIT), numberOfReaders(INITIAL_NUMBER_OF_READERS){
	pthread_mutex_init_safe(&read_bank_mutex);
	pthread_mutex_init_safe(&write_bank_mutex);
}
Bank::~Bank() {
	pthread_mutex_destroy_safe(&read_bank_mutex);
	pthread_mutex_destroy_safe(&write_bank_mutex);
}
void Bank::readLock() {
	pthread_mutex_lock_safe(&read_bank_mutex);
	this->numberOfReaders += 1;
	if (numberOfReaders == 1) {
		this->writeLock();
	}
	pthread_mutex_unlock_safe(&read_bank_mutex);
}
void Bank::readUnlock() {
	pthread_mutex_lock_safe(&read_bank_mutex);
	this->numberOfReaders -= 1;
	if (numberOfReaders == 0) {
		this->writeUnlock();
	}
	pthread_mutex_unlock_safe(&read_bank_mutex);
}
void Bank::writeLock() {
	pthread_mutex_lock_safe(&write_bank_mutex);
}
void Bank::writeUnlock() {
	pthread_mutex_unlock_safe(&write_bank_mutex);
}
void Bank::openAccount(int atm_id, int account_id, std::string password, int balance) {
	this->writeLock();
	if (this->isAccountExist(account_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else {
		Account newAccount(balance, password);
		this->mp_ac.emplace(account_id, newAccount).second;
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << atm_id << ": New account id is " << account_id << " with password " << password << " and initial balance " << balance << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	sleep(COMMAND_SLEEP_TIME_IN_SECODNS);
	this->writeUnlock();
}

void Bank::closeAccount(int atm_id, int account_id, std::string password) {
	this->writeLock();
	if (!this->isAccountExist(account_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " does not exist" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else if (!this->passwordCompare(account_id, password)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else {
		int balance = this->mp_ac[account_id].getBalance_wo_password_check();
		this->mp_ac.erase(account_id);
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << atm_id << ": Account " << account_id << " is now closed. Balance was" << balance << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	sleep(COMMAND_SLEEP_TIME_IN_SECODNS);
	this->writeUnlock();
}

int Bank::getProfit() {
	int currentProfit = 0;
	this->readLock();
	currentProfit = this->profit;
	this->readUnlock();
	return currentProfit;
}

void Bank::depositIntoAccount(int atm_id,int account_id, std::string password, int amount){
	this->readLock();
	if (!this->isAccountExist(account_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " does not exist" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else {
		this->mp_ac[account_id].deposit(atm_id, account_id, password, amount);
	}
	sleep(COMMAND_SLEEP_TIME_IN_SECODNS);
	this->readUnlock();
}
void Bank::withdrawalFromAccount(int atm_id, int account_id, std::string password, int amount) {
	this->readLock();
	if (!this->isAccountExist(account_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " does not exist" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else {
		this->mp_ac[account_id].withdrawal(atm_id, account_id, password, amount);
	}
	sleep(COMMAND_SLEEP_TIME_IN_SECODNS);
	this->readUnlock();
}

void Bank::getBalance(int atm_id, int account_id, std::string password) {
	this->readLock();
	if (!this->isAccountExist(account_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " does not exist" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else {
		this->mp_ac[account_id].getBalance(atm_id, account_id, password);
	}
	sleep(COMMAND_SLEEP_TIME_IN_SECODNS);
	this->readUnlock();
}

void Bank::transferAmount(int atm_id, int account_id,std::string password, int target_id, int amount){ 
	this->readLock();
	if (!this->isAccountExist(account_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " does not exist" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else if (!this->isAccountExist(target_id)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << target_id << " does not exist" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else if (!this->passwordCompare(account_id, password)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
	}
	else {
		if (account_id < target_id) {
			this->mp_ac[account_id].writeLock();
			this->mp_ac[target_id].writeLock();
		}
		else {
			this->mp_ac[target_id].writeLock();
			this->mp_ac[account_id].writeLock();
		}
		int account_new_balance = this->mp_ac[account_id].decreaseBalance_nolock(amount);
		if (account_new_balance == -1) {
			pthread_mutex_lock_safe(&log_mutex);
			log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " balance is lower than " << amount << std::endl;
			pthread_mutex_unlock_safe(&log_mutex);
		}
		else {
			int target_new_balance = this->mp_ac[target_id].increaseBalance_nolock(amount);
			pthread_mutex_lock_safe(&log_mutex);
			log_output_file << atm_id << ": Transfer " << amount << " from account " << account_id << " to account " << target_id <<
				" new account balance is " << account_new_balance << " new target account balance is " << target_new_balance << std::endl;
			pthread_mutex_unlock_safe(&log_mutex);
		}
		if (account_id < target_id) {
			this->mp_ac[target_id].writeUnlock();
			this->mp_ac[account_id].writeUnlock();
		}
		else {
			this->mp_ac[account_id].writeUnlock();
			this->mp_ac[target_id].writeUnlock();
		}
	}
	sleep(COMMAND_SLEEP_TIME_IN_SECODNS);
	this->readUnlock();
}

void Bank::tax(){
	int percentage = random_percentage();
	this->readLock();
	for (auto it = this->mp_ac.begin(); it != this->mp_ac.end(); ++it){
		it->second.writeLock();
		int gain = it->second.decreaseBalance_tax_nolock(percentage);
		this->profit += gain;
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Bank: commissions of " << percentage << " % were charged, the bank gained "<< gain <<" $ from account "<< it->first << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
		it->second.writeUnlock();
	}
	this->readUnlock();
}

void Bank::print() {
	this->readLock();
	for (auto it = this->mp_ac.begin(); it != this->mp_ac.end(); ++it) {
		it->second.readLock();
	}
	printf("\033[2J\033[1;1HCurrent Bank Status\n");
	for (auto it = this->mp_ac.begin(); it != this->mp_ac.end(); ++it) {
		int balance = it->second.getBalance_nolock();
		std::string password = it->second.getPassword();
		std::cout << "Account " << it->first << ": Balance - " << balance << " $, Account Password - " << password << std::endl;
	}
	std::cout << "The Bank has " << this->profit << " $\n";
	for (auto it = this->mp_ac.begin(); it != this->mp_ac.end(); ++it) {
		it->second.readUnlock();
	}
	this->readUnlock();
}

