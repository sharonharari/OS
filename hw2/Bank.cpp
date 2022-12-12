#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <unistd.h>
#include "Bank.h"
namespace fs = std::filesystem;


/*
*	Account ADT declaration
*/
Account::Account(int newBalance, int newPassword) :balance(newBalance), password(newPassword) {}
Account::~Account() {}
int Account::getBalance() const {
	return *this->balance;
}
int Account::getPassword() const {
	return *this->password;
}
bool Account::setBalance(int newBalance) :balance(newBalance) {}


/*
*	Bank ADT declaration
*/
Bank::Bank() :profit(INITIAL_BANK_PROFIT) {}
Bank::~Bank() {}

//Gil needs to review/fix/update to avoid using unneeded syscall
bool valid_args(int argc, char* argv[]) {
	if (argc == 1) {
		return false;
	}
	for (int i = 0; i < argc; i++) {
		if (!fs::exists(argv[i])) {
			return false;
		}
		if (!access(argv[i], R_OK)) {
			std::perror();
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (!valid_args(argc, argv)) {
		return 1;
	}
	return 0;
}