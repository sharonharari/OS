#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <pthread.h>
#include <cstdint>
#include <unistd.h>

class Account {
private:
	int balance;
	int password;
	int numberOfReaders;
	pthread_mutex_t read_mutex;
	pthread_mutex_t write_mutex;
public:
	Account(int balance, int password);
	~Account();
	int getBalance() const;
	int getPassword() const;
	void increaseBalance(int value);
	void decreaseBalance(int value);
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
};

#endif
