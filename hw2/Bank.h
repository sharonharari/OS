#ifndef BANK_H
#define BANK_H
#include <map>
#include <fstream>
#include <unistd.h>
#include <queue>
#include <sstream>
#include <random>
#include "Account.h"
extern pthread_mutex_t log_mutex;
extern std::ofstream log_output_file;
const int INITIAL_BANK_PROFIT = 0, MINIMUM_NUM_VALID_ARGC = 2;
const int TAX_ROUTINE_SLEEP_TIME_IN_SECONDS = 3, COMMAND_SLEEP_TIME_IN_SECODNS = 1;
const long MILLISEC_TO_NANOSEC_CONVERTOR = 1000000L;
const long PRINTOUT_ROUTINE_SLEEP_TIME_IN_MILLISEC = 500L, ATM_ROUTINE_SLEEP_TIME_IN_MILLISEC = 100L;
std::vector<std::string> split(std::string const& str, const char delim = ' ');
void milli_sleep(long milliseconds);

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
	bool passwordCompare(int account_id, std::string password);
	bool isAccountExist(int account_id);
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();
	void openAccount(int atm_id, int account_id, std::string password, int balance);
	void closeAccount(int atm_id, int account_id, std::string password);
	int getProfit();
	void depositIntoAccount(int atm_id, int account_id, std::string password, int amount);
	void withdrawalFromAccount(int atm_id, int account_id, std::string password, int amount);
	void getBalance(int atm_id, int account_id, std::string password);
	void transferAmount(int atm_id, int account_id, std::string password, int target_id, int amount);
	void tax();
	void print();
};





#endif
