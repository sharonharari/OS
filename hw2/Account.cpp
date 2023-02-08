#include "Account.h"

/*
*	pthread safe wrapper mechanism
*/
void pthread_mutex_init_safe(pthread_mutex_t* mutex) {
	if (pthread_mutex_init(mutex,NULL)) {
		std::perror("Bank error: pthread_mutex_init failed");
		log_output_file.close();
		exit(1);
	}
}

void pthread_mutex_destroy_safe(pthread_mutex_t* mutex) {
	if (pthread_mutex_destroy(mutex)) {
		std::perror("Bank error: pthread_mutex_destroy failed");
		log_output_file.close();
		exit(1);
	}
}

void pthread_mutex_lock_safe(pthread_mutex_t* mutex) {
	if (pthread_mutex_lock(mutex)) {
		std::perror("Bank error: pthread_mutex_lock failed");
		log_output_file.close();
		exit(1);
	}
}

void pthread_mutex_unlock_safe(pthread_mutex_t* mutex) {
	if (pthread_mutex_unlock(mutex)) {
		std::perror("Bank error: pthread_mutex_unlock failed");
		log_output_file.close();
		exit(1);
	}
}

/*
*	Account ADT declaration
*/
Account::Account() {
	pthread_mutex_init_safe(&read_mutex);
	pthread_mutex_init_safe(&write_mutex);
}
Account::Account(int newBalance, std::string newPassword) :balance(newBalance), password(newPassword)
	, numberOfReaders(INITIAL_NUMBER_OF_READERS) {
	pthread_mutex_init_safe(&read_mutex);
	pthread_mutex_init_safe(&write_mutex);
}

Account::~Account() {
	pthread_mutex_destroy_safe(&read_mutex);
	pthread_mutex_destroy_safe(&write_mutex);
}
void Account::getBalance(int atm_id, int account_id, std::string password) {
	if (!this->passwordCompare(password)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
		return;
	}
	this->readLock();
	pthread_mutex_lock_safe(&log_mutex);
	log_output_file << atm_id << ": Account " << account_id << " balance is " << this->balance << std::endl;
	pthread_mutex_unlock_safe(&log_mutex);
	this->readUnlock();
}
int Account::getBalance_wo_password_check() {
	int currentBalance = 0;
	this->readLock();
	currentBalance = this->balance;
	this->readUnlock();
	return currentBalance;
}
int Account::getBalance_nolock() {
	return this->balance;
}

std::string Account::getPassword() const {
	return this->password;
}
bool Account::passwordCompare(std::string password) const {
	return (this->getPassword() == password);
}

void Account::deposit(int atm_id, int account_id, std::string password, int amount) {
	if (!this->passwordCompare(password)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
		return;
	}
	this->writeLock();
	int new_balance = this->balance + amount;
	this->balance = new_balance;
	pthread_mutex_lock_safe(&log_mutex);
	log_output_file << atm_id << ": Account " << account_id << " new balance is " << new_balance << " after " << amount << " $ was deposited" << std::endl;
	pthread_mutex_unlock_safe(&log_mutex);
	this->writeUnlock();
}

void Account::withdrawal(int atm_id, int account_id, std::string password, int amount) {
	if (!this->passwordCompare(password)) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
		return;
	}
	this->writeLock();
	if (this->balance < amount) {
		pthread_mutex_lock_safe(&log_mutex);
		log_output_file << "Error " << atm_id << ": Your transaction failed - account id " << account_id << " balance is lower than " << amount << std::endl;
		pthread_mutex_unlock_safe(&log_mutex);
		this->writeUnlock();
		return;
	}
	int new_balance = this->balance - amount;
	this->balance = new_balance;
	pthread_mutex_lock_safe(&log_mutex);
	log_output_file << atm_id << ": Account " << account_id << " new balance is " << new_balance << " after " << amount << " $ was withdrew" << std::endl;
	pthread_mutex_unlock_safe(&log_mutex);
	this->writeUnlock();
}
int Account::increaseBalance_nolock(int value) {
	int new_balance;
	new_balance = this->balance + value;
	this->balance = new_balance;
	return new_balance;
}
int Account::decreaseBalance_nolock(int value) {
	if(this->balance < value){
		return -1;
	}
	int new_balance;
	new_balance = this->balance - value;
	this->balance = new_balance;
	return new_balance;
}
int Account::decreaseBalance_tax_nolock(int percentage) {
	int gain = (int)(std::round(((this->balance) * percentage) / 100.0));
	this->balance -= gain;
	return gain;
}
void Account::readLock() {
	pthread_mutex_lock_safe(&read_mutex);
	this->numberOfReaders += 1;
	if (this->numberOfReaders == 1) {
		this->writeLock();
	}
	pthread_mutex_unlock_safe(&read_mutex);
}
void Account::readUnlock() {
	pthread_mutex_lock_safe(&read_mutex);
	this->numberOfReaders -= 1;
	if (this->numberOfReaders == 0) {
		this->writeUnlock();
	}
	pthread_mutex_unlock_safe(&read_mutex);
}
void Account::writeLock() {
	pthread_mutex_lock_safe(&write_mutex);
}
void Account::writeUnlock() {
	pthread_mutex_unlock_safe(&write_mutex);
}