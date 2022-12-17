#include "Bank.h"

std::vector<std::string> split(std::string const& str, const char delim)
{
	std::istringstream split(str);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, delim); tokens.push_back(each));
	return tokens;
}

/*
*	Bank ADT definition
*/
bool Bank::passwordCompare(int account_id, int password) {
	return (this->mp_ac[account_id].getPassword() == password);
}
bool Bank::isAccountExist(int account_id) {
	return (!(this->mp_ac.find(account_id) == this->mp_ac.end()));
}
Bank::Bank() :profit(INITIAL_BANK_PROFIT), numberOfReaders(INITIAL_NUMBER_OF_READERS){
	pthread_mutex_init(&read_bank_mutex, NULL);
	pthread_mutex_init(&write_bank_mutex, NULL);
}
Bank::~Bank() {
	pthread_mutex_destroy(&read_bank_mutex);
	pthread_mutex_destroy(&write_bank_mutex);
}
void Bank::readLock() {
	pthread_mutex_lock(&read_bank_mutex);
	this->numberOfReaders += 1;
	if (numberOfReaders == 1) {
		this->writeLock();
	}
	pthread_mutex_unlock(&read_bank_mutex);
}
void Bank::readUnlock() {
	pthread_mutex_lock(&read_bank_mutex);
	this->numberOfReaders -= 1;
	if (numberOfReaders == 0) {
		this->writeUnlock();
	}
	pthread_mutex_unlock(&read_bank_mutex);
}
void Bank::writeLock() {
	pthread_mutex_lock(&write_bank_mutex);
}
void Bank::writeUnlock() {
	pthread_mutex_unlock(&write_bank_mutex);
}
bool Bank::openAccount(int account_id, int balance, int password) {
	Account newAccount(balance, password);
	bool newAccountHasAdded = this->mp_ac.emplace(account_id, newAccount).second;
	return newAccountHasAdded;
}

void Bank::closeAccount(int account_id, int password) {//Need more revision
	this->mp_ac.erase(account_id); //Should be locked as critical section?
}

int Bank::getProfit() {
	int currentProfit = 0;
	this->readLock();
	currentProfit = this->profit;
	this->readUnlock();
	return currentProfit;
}

int Bank::depositIntoAccount(int account_id, int password, int amount){ 
	return (this->mp_ac[account_id].increaseBalance(amount));
}
bool Bank::withdrawalFromAccount(int account_id, int password, int amount){ 
	return (this->mp_ac[account_id].decreaseBalance(amount));
}

int Bank::getBalance(int account_id, int password){ 
	return (this->mp_ac[account_id].getBalance());
}

bool Bank::transferAmount(int account_id, int password, int target_id, int amount, int *newAccountBalance, int *newTargetBalance){ 
	this->mp_ac[account_id].writeLock();
	this->mp_ac[target_id].writeLock();
	*newAccountBalance = this->mp_ac[account_id].decreaseBalance_nolock(amount);
	if((*newAccountBalance) == -1){
		this->mp_ac[account_id].writeUnlock();
		this->mp_ac[target_id].writeUnlock();
		return false;
	}
	*newTargetBalance = this->mp_ac[target_id].increaseBalance_nolock(amount);
	this->mp_ac[account_id].writeUnlock();
	this->mp_ac[target_id].writeUnlock();
	return true;
}

void Bank::tax(){
	// int ratio = (std::experimental::randint(1, 5))/100; // Check if it compiles C11
	int ratio = 3/100;
	for (auto it = mp_ac.begin(); it != mp_ac.end(); ++it){
		this->writeLock();
		it->second.writeLock();
		int gain = it->second.decreaseBalance_tax_nolock(ratio);
		this->profit += gain;
		std::cout << "Bank: commissions of " << ratio*100 << " % were charged, the bank gained "<< gain <<" $ from account "<< it->first << std::endl;
		it->second.writeUnlock();
		this->writeUnlock();
	}
}

