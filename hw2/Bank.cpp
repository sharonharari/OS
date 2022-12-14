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
	pthread_mutex_init(&read_mutex, NULL);
	pthread_mutex_init(&write_mutex, NULL);
}
Bank::~Bank() {
	pthread_mutex_destroy(&read_mutex);
	pthread_mutex_destroy(&write_mutex);
}
void Bank::readLock() {
	pthread_mutex_lock(&read_mutex);
	this->numberOfReaders += 1;
	if (numberOfReaders == 1) {
		this->writeLock();
	}
	pthread_mutex_unlock(&read_mutex);
}
void Bank::readUnlock() {
	pthread_mutex_lock(&read_mutex);
	this->numberOfReaders -= 1;
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




std::queue<std::vector<std::string>>* valid_args(int argc, char* argv[]) {
	if (argc < MINIMUM_NUM_VALID_ARGC) {
		return NULL;
	}
	std::queue<std::vector<std::string>>* input_files = new std::queue<std::vector<std::string>>[argc - 1];
	for (int i = 1; i < argc; i++) { //argv[0] == program name
		std::ifstream tempFile(argv[i]);
		if (!tempFile.is_open())//file doesn't exist or can't be read from
		{
			delete[] input_files;
			return NULL;
		}
		std::string line;
		while (std::getline(tempFile, line)) {
			input_files[i - 1].push(split(line));
		}
		tempFile.close();
	}
	return input_files;
}

int main(int argc, char* argv[]) {
	std::queue<std::vector<std::string>>* input_files = valid_args(argc, argv);
	if (!input_files) {
		return 1;
	}
	delete[] input_files;
	return 0;
}