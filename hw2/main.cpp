#include "Bank.h"
#include "Account.h"
#include <ctime>
Bank bank;
void *atm(void *arg);
void* tax_routine(void* arg);
void* printout_routine(void* arg);
bool finished = false;

void milli_sleep(long milliseconds) {
    long arg_in_nanoseconds = milliseconds * MILLISEC_TO_NANOSEC_CONVERTOR;
    struct timespec interval = { 0 };
    interval.tv_sec = 0;
    interval.tv_nsec = arg_in_nanoseconds;
    if (nanosleep(&interval, NULL)) {
        std::perror("Bank error: nanosleep failed");
    }
}

class atm_input{
public:
	int id;
    std::queue<std::vector<std::string>> file;
};
std::queue<std::vector<std::string>>* valid_args(int argc, char* argv[]);

pthread_mutex_t log_mutex;

int main(int argc, char* argv[]) {
    pthread_mutex_init(&log_mutex, NULL);
	std::queue<std::vector<std::string>>* input_files = valid_args(argc, argv);
	if (!input_files) {
		std::cerr << "Bank error: illegal arguments" << std::endl;
        pthread_mutex_destroy(&log_mutex);
		return 1;
	}
    // Bank printout thread create
    int bank_id = 0;
    pthread_t bank_printout_thread;
    if(pthread_create( &bank_printout_thread, NULL, printout_routine, (void *)&bank_id)){
        std::perror("Bank error: pthread_create failed");
    }
    // Bank tax thread create
    int bank_tax_id = 0;
    pthread_t bank_tax_thread;
    if (pthread_create(&bank_tax_thread, NULL, tax_routine, (void*)&bank_tax_id)) {
        std::perror("Bank error: pthread_create failed");
    }
    // ATMs threads create
    pthread_t* atms_threads = new pthread_t[argc - 1];
    atm_input* atm_inputs = new atm_input[argc - 1];
    for(int i = 0; i < argc-1; i++) {
        atm_inputs[i].id = i; 
        atm_inputs[i].file = input_files[i];
        if(pthread_create( &atms_threads[i], NULL, atm, (void *)&atm_inputs[i])){
            std::perror("Bank error: pthread_create failed");
        }
    }
    // ATMs threads join
    for(int i = 0; i < argc-1; i++) {
        if (pthread_join(atms_threads[i], NULL)) {
            std::perror("Bank error: pthread_join failed");
        }
    } 
    finished = true;
    if (pthread_join(bank_tax_thread, NULL)) {
        std::perror("Bank error: pthread_join failed");
    }
    if (pthread_join(bank_printout_thread, NULL)) {
        std::perror("Bank error: pthread_join failed");
    }
    delete[] input_files;
    delete[] atm_inputs;
    pthread_mutex_destroy(&log_mutex);
    return 0;
}

void *tax_routine(void *arg){
    while(!finished){
        bank.tax();
        sleep(TAX_ROUTINE_SLEEP_TIME_IN_SECONDS);
    }
    pthread_exit(NULL);
}

void* printout_routine(void* arg) {
    while (!finished) {
        bank.print();
        milli_sleep(PRINTOUT_ROUTINE_SLEEP_TIME_IN_MILLISEC);
    }
    pthread_exit(NULL);
}

void *atm(void *arg){
    atm_input input = *(atm_input *)arg;
    while(!input.file.empty()){
        milli_sleep(ATM_ROUTINE_SLEEP_TIME_IN_MILLISEC);
        std::vector<std::string> cmd = input.file.front();
        input.file.pop();
        int atm_id = input.id;
        int account_id = std::stoi(cmd[1]);
        std::string password(cmd[2]);
        if(cmd[0] == "O"){ // Open account
            int initial_amount = std::stoi(cmd[3]);
            bank.openAccount(atm_id, account_id, password,initial_amount);
        }
        else if(cmd[0] == "D"){ // Deposit
            int amount = std::stoi(cmd[3]);
            bank.depositIntoAccount(atm_id,account_id, password, amount);
        }
        else if(cmd[0] == "W"){ // Withdrawl
            int amount = std::stoi(cmd[3]);
            bank.withdrawalFromAccount(atm_id, account_id, password, amount);

        }
        else if(cmd[0] == "B"){ // get Balance
            bank.getBalance(atm_id, account_id, password);
        }
        else if(cmd[0] == "Q"){ // Close account
            bank.closeAccount(atm_id, account_id, password);
        }
        else if (cmd[0] == "T") { // Transfer
            int target_id = std::stoi(cmd[3]);
            int amount = std::stoi(cmd[4]);
            bank.transferAmount(atm_id, account_id, password, target_id, amount);
        }
        else {
            // error - shouldn't get here
        }
        milli_sleep(ATM_ROUTINE_SLEEP_TIME_IN_MILLISEC);
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