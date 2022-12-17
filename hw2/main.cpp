#include "Bank.h"
#include "Account.h"

Bank bank;
void *atm(void *arg);
bool finished = false;

class atm_input{
public:
	int id;
    std::queue<std::vector<std::string>> file;
};
std::queue<std::vector<std::string>>* valid_args(int argc, char* argv[]);



int main(int argc, char* argv[]) {
	std::queue<std::vector<std::string>>* input_files = valid_args(argc, argv);
	if (!input_files) {
		std::cerr << "Bank error: illegal arguments" << std::endl;
		return 1;
	}

    pthread_t *threads = new pthread_t[argc]; // 0 is the bank, 1-N is the ATMs
    atm_input *atm_inputs = new atm_input[argc];

    // Bank thread create
    int bank_id = 0;
    if(!pthread_create( &threads[0], NULL, bank_routine, (void *)&bank_id)){
        std::perror("Bank error: pthread_create failed");
    }
    // ATMs threads create
    for(int i = 1; i < argc; i++) {
        atm_inputs[i].id = i; 
        atm_inputs[i].file = input_files[i-1];
        if(!pthread_create( &threads[i], NULL, atm, (void *)&atm_inputs[i])){
            std::perror("Bank error: pthread_create failed");
        }
    }

    for(int i = 1; i < argc; i++) {
        pthread_join(threads[i], NULL);
    } 
    finished = true;

    pthread_join(threads[0], NULL);
    delete[] input_files;
    return 0;
}

void *bank_routine(void *bank_id){
    while(!finished){
        sleep(3);
        bank.tax();
    }
}

void *atm(void *arg){
    atm_input input = *(atm_input *)arg;
    while(!input.file.empty()){
        std::vector<std::string> cmd = input.file.front();
        input.file.pop();
        // Assuming cmd entered is valid
        if(cmd[0] == "O"){ // Open account
            bank.writeLock();
            sleep(1);
            if(bank.isAccountExist(std::stoi(cmd[1]))){
                std::cerr << "Error "<< input.id <<": Your transaction failed - account with the same id exists" << std::endl;
            }
            else{
                bank.openAccount(std::stoi(cmd[1]),std::stoi(cmd[3]),std::stoi(cmd[2]));
                std::cout <<input.id<<": New account id is "<<cmd[1] <<" with password "<< cmd[2] <<" and initial balance "<< cmd[3] <<std::endl;
            }
            bank.writeUnlock();
        }
        else if(cmd[0] == "D"){ // Deposit
            bank.readLock();
            int account_id = std::stoi(cmd[1]);
            int password = std::stoi(cmd[2]);
            int amount = std::stoi(cmd[3]);
            int new_balance;
            sleep(1);
            if(!bank.isAccountExist(account_id)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< account_id <<" does not exist"<< std::endl;
            }
            else if(!bank.passwordCompare(account_id,password)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - password for account id "<< account_id <<" is incorrect" << std::endl;
            }
            else {
                new_balance =  bank.depositIntoAccount(account_id,password,amount);
                std::cout << input.id << ": Account " <<account_id <<" new balance is "<< new_balance <<" after "<< amount <<" $ was deposited" << std::endl;
            }
            bank.readUnlock();
        }
        else if(cmd[0] == "W"){ // Withdrawl
            bank.readLock();
            int account_id = std::stoi(cmd[1]);
            int password = std::stoi(cmd[2]);
            int amount = std::stoi(cmd[3]);
            int new_balance;
            sleep(1);
            if(!bank.isAccountExist(account_id)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< account_id <<" does not exist"<< std::endl;
            }
            else if(!bank.passwordCompare(account_id,password)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - password for account id "<< account_id <<" is incorrect" << std::endl;
            }
            else {
                new_balance =  bank.withdrawalFromAccount(account_id,password,amount);
                if (new_balance == -1){
                    std::cerr << "Error "<< input.id <<": Your transaction failed - account id "<< account_id <<" balance is lower than "<< amount << std::endl;
                }
                else{
                    std::cout << input.id << ": Account " <<account_id <<" new balance is "<< new_balance <<" after "<< amount <<" $ was withdrew" << std::endl;
                }
            }
            bank.readUnlock();
        }
        else if(cmd[0] == "B"){ // get Balance
            bank.readLock();
            int account_id = std::stoi(cmd[1]);
            int password = std::stoi(cmd[2]);
            int balance;
            sleep(1);
            if(!bank.isAccountExist(account_id)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< account_id <<" does not exist"<< std::endl;
            }
            else if(!bank.passwordCompare(account_id,password)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - password for account id "<< account_id <<" is incorrect" << std::endl;
            }
            else{
                balance = bank.getBalance(account_id,password);
                std::cout << input.id << ": Account " << account_id << "balance is " << balance << std::endl;
            }
            bank.readUnlock();
        }
        else if(cmd[0] == "Q"){ // Close account
            bank.writeLock();
            sleep(1);
            int account_id = std::stoi(cmd[1]);
            int password = std::stoi(cmd[2]);
            if(!bank.isAccountExist(account_id)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< account_id <<" does not exist"<< std::endl;
            }
            else if(!bank.passwordCompare(account_id,password)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - password for account id "<< account_id <<" is incorrect" << std::endl;
            }
            else{
                bank.closeAccount(account_id,password);
            }
            bank.writeUnlock();
        }
        else if(cmd[0] == "T"){ // Transfer
            bank.readLock();
            sleep(1);
            int account_id = std::stoi(cmd[1]);
            int password = std::stoi(cmd[2]);
            int target_id = std::stoi(cmd[3]);
            int amount = std::stoi(cmd[4]);
            int account_new_balance, target_new_balance;
            if(!bank.isAccountExist(account_id)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< account_id <<" does not exist"<< std::endl;
            }
            else if(!bank.isAccountExist(target_id)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< target_id <<" does not exist"<< std::endl;
            }
            else if(!bank.passwordCompare(account_id,password)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - password for account id "<< account_id <<" is incorrect" << std::endl;
            }
            else if(!bank.transferAmount(account_id,password,target_id,amount,&account_new_balance, &target_new_balance)){
                std::cerr << "Error "<< input.id << ": Your transaction failed - account id "<< account_id <<" balance is lower than "<< amount << std::endl;
            }
            else{
                std::cout << input.id << ": Transfer "<< amount << " from account "<< account_id << " to account "<< target_id <<
                 " new account balance is " << account_new_balance<< " new target account balance is "<< target_new_balance << std::endl;
            }
            bank.readUnlock();
        }
        else {
            // error - shouldn't get here
        }
    }
    
    pthread_exit(NULL); 
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