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
#include <experimental/random>
#include "Account.h"

const int INITIAL_BANK_PROFIT = 0, MINIMUM_NUM_VALID_ARGC = 2;
std::vector<std::string> split(std::string const& str, const char delim = ' ');

class Bank {
private:
	std::map<int, Account, std::less<int>> mp_ac;
	int profit;
	int numberOfReaders;
	pthread_mutex_t read_mutex;
	pthread_mutex_t write_mutex;
	pthread_mutex_t open_close_account_mutex;
public:
	Bank();
	~Bank();
	bool passwordCompare(int account_id, int password);
	bool isAccountExist(int account_id);
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
	void open_close_accountLock();
	void open_close_accountUnlock();
	bool openAccount(int account_id,int balance, int password);
	void closeAccount(int account_id, int password);
	void depositIntoAccount(int account_id, int password, int amount);
	bool withdrawalFromAccount(int account_id, int password, int amount);
	int getBalance(int account_id, int password);
	bool transferAmount(int account_id, int password, int target_id, int amount);
	int getProfit();
	bool tax();
};





#endif
