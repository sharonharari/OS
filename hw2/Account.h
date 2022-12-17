#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <pthread.h>
#include <cstdint>
#include <unistd.h>
#include <cmath>

const int INITIAL_NUMBER_OF_READERS = 0;

class Account {
private:
	int balance;
	int password;
	int numberOfReaders;
	pthread_mutex_t read_mutex;
	pthread_mutex_t write_mutex;
public:
	Account();
	Account(int newBalance, int newPassword);
	~Account();
	int getBalance();
	int getPassword() const;
	int increaseBalance(int value);
	int increaseBalance_nolock(int value);
	int decreaseBalance(int value);
	int decreaseBalance_nolock(int value);
	int decreaseBalance_tax_nolock(int percentage);
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
};

#endif
