#ifndef BANK_H
#define BANK_H
#include <map>
#include <pthread.h>
#include <iostream> 
#include <fstream> 
#include <string>
#include <cstdint>
#include <unistd.h>
#include <queue>
#include <sstream>
//#include <experimental/random>
#include "Account.h"

const int INITIAL_BANK_PROFIT = 0, MINIMUM_NUM_VALID_ARGC = 2;
std::vector<std::string> split(std::string const& str, const char delim = ' ');

class Bank {
private:
	std::map<int, Account, std::less<int>> mp_ac;
	int profit;
	int numberOfReaders;
	pthread_mutex_t read_bank_mutex;
	pthread_mutex_t write_bank_mutex;
public:
	Bank();
	~Bank();
	bool passwordCompare(int account_id, int password);
	bool isAccountExist(int account_id);
	// profit lock
	// void read_profitLock();
	// void read_profitUnlock();
	// void write_profitLock();
	// void write_profitUnlock();
	// add/close account lock
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();

	bool openAccount(int account_id,int balance, int password);
	void closeAccount(int account_id, int password);
	int depositIntoAccount(int account_id, int password, int amount);
	bool withdrawalFromAccount(int account_id, int password, int amount);
	int getBalance(int account_id, int password);
	bool transferAmount(int account_id, int password, int target_id, int amount, int *newAccountBalance, int *newTargetBalance);
	int getProfit();
	void tax();
};





#endif
