#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <pthread.h>
#include <iostream>
#include <cstdint>
#include <unistd.h>
#include <cmath>
#include <string>

const int INITIAL_NUMBER_OF_READERS = 0;
extern pthread_mutex_t log_mutex;
class Account {
private:
	int balance;
	std::string password;
	int numberOfReaders;
	pthread_mutex_t read_mutex;
	pthread_mutex_t write_mutex;
public:
	Account();
	Account(int newBalance, std::string newPassword);
	~Account();
	void getBalance(int atm_id, int account_id, std::string password);
	int getBalance_wo_password_check();
	int getBalance_nolock();
	std::string getPassword() const;
	bool passwordCompare(std::string password) const;
	void deposit(int atm_id, int account_id, std::string password, int amount);
	void withdrawal(int atm_id, int account_id, std::string password, int amount);
	//int increaseBalance(int value);
	int increaseBalance_nolock(int value);
	//int decreaseBalance(int value);
	int decreaseBalance_nolock(int value);
	int decreaseBalance_tax_nolock(int percentage);
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
};

#endif
