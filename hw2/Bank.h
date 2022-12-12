#ifndef BANK_H
#define BANK_H
#include <map>
#include <pthread.h>

const int INITIAL_BANK_PROFIT = 0;

class Account {
private:
	int balance;
	int password;
public:
	Account(int balance, int password);
	~Account();
	int getBalance() const;
	int getPassword() const;
	bool setBalance(int newBalance);
};


class Bank {
private:
	std::map<int, Account, std::less<int>> mp_ac;
	int profit;
	std::map<int, pthread_ mutex_ t, std::less<int>> mutex;
	bool passwordCompare(int account_id, int password);
public:
	Bank();
	~Bank();
	bool openAccount(int account_id,int balance, int password);
	bool closeAccount(int account_id, int password);
	bool depositIntoAccount(int account_id, int password, int amount);
	bool withdrawalFromAccount(int account_id, int password, int amount);
	void getBalance(int account_id, int password) const;
	bool transferAmount(int account_id, int password, int target_id, int amount);
	bool tax();
};





#endif
