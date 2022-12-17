#include "Account.h"


/*
*	Account ADT declaration
*/
Account::Account() {
	pthread_mutex_init(&read_mutex, NULL);
	pthread_mutex_init(&write_mutex, NULL);
}
Account::Account(int newBalance, int newPassword) :balance(newBalance), password(newPassword)
	, numberOfReaders(INITIAL_NUMBER_OF_READERS) {
	pthread_mutex_init(&read_mutex, NULL);
	pthread_mutex_init(&write_mutex, NULL);
}

Account::~Account() {
	pthread_mutex_destroy(&read_mutex);
	pthread_mutex_destroy(&write_mutex);
}
int Account::getBalance() {
	int currentBalance = 0;
	this->readLock();
	currentBalance = this->balance;
	this->readUnlock();
	return currentBalance;
}
int Account::getPassword() const {
	return this->password;
}
int Account::increaseBalance(int value) {
	this->writeLock();
	int new_balance = this->balance +value;
	this->balance = new_balance;
	this->writeUnlock();
	return new_balance;
}
int Account::decreaseBalance(int value) {
	this->writeLock();
	if(this->balance < value){
		this->writeUnlock();
		return -1;
	}
	int new_balance = this->balance - value;
	this->balance = new_balance;
	this->writeUnlock();
	return new_balance;
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
int Account::decreaseBalance_tax_nolock(int ratio) {
	int gain = (this->balance)*ratio;
	this->balance -= gain;
	return gain;
}
void Account::readLock() {
	pthread_mutex_lock(&read_mutex);
	this->numberOfReaders += 1;
	if (this->numberOfReaders == 1) {
		this->writeLock();
	}
	pthread_mutex_unlock(&read_mutex);
}
void Account::readUnlock() {
	pthread_mutex_lock(&read_mutex);
	this->numberOfReaders -= 1;
	if (this->numberOfReaders == 0) {
		this->writeUnlock();
	}
	pthread_mutex_unlock(&read_mutex);
}
void Account::writeLock() {
	pthread_mutex_lock(&write_mutex);
}
void Account::writeUnlock() {
	pthread_mutex_unlock(&write_mutex);
}