#ifndef BANK_H
#define BANK_H
#include <map>
#include <pthread.h>
#include <iostream> 
#include <fstream> 
#include <string>
#include <cstdint>
#include <unistd.h>
#include "Account.h"

const int INITIAL_BANK_PROFIT = 0, MINIMUM_NUM_VALID_ARGC = 2;

class Bank {
private:
	std::map<int, Account, std::less<int>> mp_ac;
	int profit;
	pthread_mutex_t read_mutex;
	pthread_mutex_t write_mutex;
public:
	Bank();
	~Bank();
	bool passwordCompare(int account_id, int password);
	bool isAccountExist(int account_id);
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
	bool openAccount(int account_id,int balance, int password);
	void closeAccount(int account_id, int password);
	bool depositIntoAccount(int account_id, int password, int amount);
	bool withdrawalFromAccount(int account_id, int password, int amount);
	void getBalance(int account_id, int password) const;
	bool transferAmount(int account_id, int password, int target_id, int amount);
	int getProfit() const;
	bool tax();
};





#endif
