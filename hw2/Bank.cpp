#include "Bank.h"


/*
*	Bank ADT definition
*/
bool Bank::passwordCompare(int account_id, int password) {
	return (mp_ac[account_id].getPassword() == password);
}
bool Bank::isAccountExist(int account_id) {
	return (!(this->mp_ac.find(account_id) == this->mp_ac.end()));
}
Bank::Bank() :profit(INITIAL_BANK_PROFIT) {
	pthread_mutex_init(&read_mutex, NULL);
	pthread_mutex_init(&write_mutex, NULL);
}
Bank::~Bank() {
	pthread_mutex_destroy(&read_mutex);
	pthread_mutex_destroy(&write_mutex);
}
void Bank::readLock() {
	pthread_mutex_lock(&read_mutex);
	numberOfReaders += 1;
	if (numberOfReaders == 1) {
		this->writeLock();
	}
	pthread_mutex_unlock(&read_mutex);
}
void Bank::readUnlock() {
	pthread_mutex_lock(&read_mutex);
	numberOfReaders -= 1;
	if (numberOfReaders == 0) {
		this->writeUnlock();
	}
	pthread_mutex_unlock(&read_mutex);
}
void Bank::writeLock() {
	pthread_mutex_lock(&write_mutex);
}
void Bank::writeUnlock() {
	pthread_mutex_unlock(&write_mutex);
}
bool Bank::openAccount(int account_id, int balance, int password) {
	Account newAccount(balance, password);
	bool newAccountHasAdded = this->mp_ac.insert(std::pair<int, Account>(account_id, newAccount)).second;
	return newAccountHasAdded;
}

void Bank::closeAccount(int account_id, int password) {//Need more revision
	this->mp_ac.erase(account_id); //Should be locked as critical section?
}

int Bank::getProfit() const {
	int currentProfit = 0;
	this->readLock();
	currentProfit = this->profit;
	this->readUnlock();
	return currentProfit;
}



//Gil needs to review/fix/update to avoid using unneeded syscall
bool valid_args(int argc, char* argv[]) {
	if (argc < MINIMUM_NUM_VALID_ARGC) {
		return false;
	}
	std::ifstream* input_files = new std::ifstream[argc - 1];
	for (int i = 1; i < argc; i++) { //argv[0] == program name
		/*if (!fs::exists(argv[i])) {
			return false;
		} //Only from cpp17
		if (!access(argv[i], R_OK)) {
			std::perror();
			return false;
		} //Syscall we try to not use.*/
		std::ifstream tempFile(argv[i]);
		if (!tempFile.is_open())//file doesn't exist or can't be read from
		{
			for (int j = 1; j < i; j++) {
				input_files[j].close();
			}
			delete[] input_files;
			return false;
		}
		input_files[i - 1] = tempFile;
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (!valid_args(argc, argv)) {
		return 1;
	}
	return 0;
}