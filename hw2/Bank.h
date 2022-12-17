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
#include <random>
#include "Account.h"

const int INITIAL_BANK_PROFIT = 0, MINIMUM_NUM_VALID_ARGC = 2;
const int TAX_ROUTINE_SLEEP_TIME_IN_SECONDS = 3, COMMAND_SLEEP_TIME_IN_SECODNS = 1;
const long MILLISEC_TO_NANOSEC_CONVERTOR = 1000000L;
const long PRINTOUT_ROUTINE_SLEEP_TIME_IN_MILLISEC = 500L, ATM_ROUTINE_SLEEP_TIME_IN_MILLISEC = 100L;
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
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
	bool openAccount(int account_id,int balance, int password);
	void closeAccount(int account_id, int password);
	int depositIntoAccount(int account_id, int password, int amount);
	int withdrawalFromAccount(int account_id, int password, int amount);
	int getBalance(int account_id);
	bool transferAmount(int account_id, int password, int target_id, int amount, int *newAccountBalance, int *newTargetBalance);
	int getProfit();
	void tax();
	void print();
};





#endif
