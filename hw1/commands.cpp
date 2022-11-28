//		commands.c
//********************************************
const int CMD = 0;
const int SUCCESS = 0;
const int FAILED = -1;
#include <iostream>
#include "commands.h"
#include <string>
#include<map>
//#include <bits/basic_string.h>//using namespace std;
//sing std::a;
//using std::endl;
//using std::string;
/*
class job{
public:
	pid_t pid;
	std::string cmd;
	job_state state;
	time_t entered_time; // the time the job entered the list
	job(pid_t pida,std::string cmda,job_state statea,time_t entered_timea);
	~job();
};*/
job::job(pid_t pida,std::string cmda,job_state statea,time_t entered_timea)
{
		pid=pida;
		cmd=cmda;
		state=statea;
		entered_time=entered_timea;
}
job::~job(){}

bool is_number(std::string& str);
bool is_number_char(char * str);
int arg_in_map(char * arg);


bool is_number(std::string& str);
bool is_number_char(char * str);
int arg_in_map(std::string& arg);



//********************************************
int find_stopped()
{
	job stopped = NULL;
	for (auto it = mp.begin(); it != mp.end(); ++it)
		if ((it->second).state == Stopped)
			stopped = it->first;
	if(stopped == NULL) return -1;

	return stopped.pid;
}
//********************************************


int cmdline_split_into_arguments(std::string line, std::string(&args)[MAX_ARG], std::string delimiters = " \t\n")
{

	char* iter = strtok(const_cast<char*>(line.c_str()), const_cast<char*>(delimiters.c_str()));
	int i = 0, num_arg = 0;
	if (iter == NULL)
		return 0;
	args[0] = iter; // arg[0] is the cmd
	for (i = 1; i < MAX_ARG; i++)
	{
		iter = strtok(NULL, const_cast<char*>(delimiters.c_str())); // arg[i] is the arguments by order
		if (iter != NULL) {
			args[i] = iter;
			num_arg++;  // number of arguments of cmd
		}

	}
	return num_arg;
}



// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, std::string args[MAX_ARG], int num_args, std::string cmdString)
{
	
	bool illegal_cmd = false; // illegal command
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (args[CMD] == "cd") //
	{
		if (num_args > 1) 
			perror("smash error: cd: too many arguments");
		else if (args[1] == "-") {
			if (last_path == NULL) 
				perror("smash error: cd: OLDPWD not set"); // TBD initialize to NULL
			else if (chdir(last_path)) {
				perror("chdir() error");
			}
		}
		else {
			if (chdir(const_cast<char*>(args[1].c_str()))) perror("chdir() error");
			else {
				char* last_path_tmp = get_current_dir_name();
				if (last_path_tmp == NULL)
					perror("getcwd() error");
				free(last_path);
				// maybe change it to string instead char *
				last_path = last_path_tmp;
			}
		}
	} 

	
	/*************************************************/
	else if (args[CMD] == "pwd")  //
	{
		char * cwd = get_current_dir_name();
		if ( cwd == NULL)
			perror("smash error: getcwd() error");
		else
		    printf("%s", cwd);
		free(cwd);
	}
	
	/*************************************************/
	else if (args[CMD] == "kill")
	{
		// valid arguments check
		if (num_args != 2) {
			perror("smash error: kill: invalid arguments");
		}
		else{
			std::string signum_s(args[1]);
			std::string job_id_s(args[2]);
			if (signum_s[0] != '-'){
				perror("smash error: kill: invalid arguments");
			}
			else signum_s.erase(0,1);
			if (!is_number(signum_s) || !is_number(job_id_s) ){
				perror("smash error: kill: invalid arguments");
			}
			else {
				// job_id check
				int job_id = std::stoi(job_id_s);
				int signum = std::stoi(signum_s);
				if(mp.find(job_id) == mp.end()){
					std::cerr << "smash error: kill: job-id " <<job_id<<" does not exist" << std::endl;
				}
				else {
					if (kill(mp[job_id].pid , signum)){
						printf("smash error: kill: job-id %d - Cannot send signal\n",job_id);
					}
					std::cout << "signal number "<< signum<< " was sent to pid "<<mp[job_id].pid << std::endl;
				}
			}
		}
	}
	else if (args[CMD] == "diff")
	{
		if (num_args != 2) {
			perror("smash error: diff: invalid arguments");
		}
		else {
			FILE *f1, *f2;
			f1 = fopen(const_cast<char*>(args[1].c_str()),"r");
			if( f1 == NULL){
				std::cerr << "smash error : failed to open file" << std::endl;
				return 1;
			}
			f2 = fopen(const_cast<char*>(args[2].c_str()),"r");
			if (f2 == NULL){
				std::cerr << "smash error : failed to open file" << std::endl;
				return 1;
			}
			unsigned char c1 = 0;
			unsigned char c2 = 0;
			bool diff = 0;
			while (!feof(f1) || !feof(f2)){
				c1 = getc(f1);
				c2 = getc(f2);
				if (c1 != c2) {
					diff = 1;
					break;
				}
			}
			if (diff){ //different
				std::cout << "1" << std::endl;
			}
			else{ //identical
				std::cout << "0" << std::endl;
			}
			fclose(f1);
			fclose(f2);
		}
	}
	/*************************************************/
	
	else if (args[CMD] == "jobs") //
	{
		time_t curr_time(time(NULL));
		double diff_time;
		//for (auto it : mp){
		for (auto it = mp.begin(); it != mp.end(); ++it){
			 diff_time = difftime(curr_time, (it->second).entered_time);
			 std::cout << "[" << it->first << "] " << (it->second).cmd
					 << " : " << (it->second).pid << " "
					 << diff_time << "secs";
			 if((it->second).state == Stopped){
				 std::cout << " (stopped)" << std::endl;
			 }
		}
	}
	/*************************************************/
	else if (args[CMD] == "showpid")  //
	{
		std::cout << "smash pid is " << getpid() << std::endl;
	}
	/*************************************************/
	else if (args[CMD] == "fg")  //
	{
		if(num_args > 1){
			// too many arguments
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
		}
		else if (num_args == 0 && mp.empty()) {
			// no arguments passed, and map is empty
			std::cerr << "smash error: fg: jobs list is empty" << std::endl;
		}
		else if (num_args != 0 && !is_number(args[1])){
			// argument is not a number - invalid argument
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
		}
		else if ((num_args != 0) && (arg_in_map(args[1])==-1)) {
			// job id is not found
			std::cerr << "smash error: fg: job-id" << args[1] << " does not exist" << std::endl;
		} else {
			// no error
			int job_id = -1;
			if (num_args == 0) { // take the biggest job id
				job_id = mp.rbegin()->first; // the biggest key
			}
			else job_id = arg_in_map(args[1]); // take the arg job_id
			std::cout << mp[job_id].cmd << " : " << mp[job_id].pid << std::endl;

			kill(mp[job_id].pid, SIGCONT);
			// wait for job to finish - running in foreground
			waitpid(mp[job_id].pid, NULL, WCONTINUED);
			mp.erase(job_id);
		}
	} 
	/*************************************************/
	else if (args[CMD] == "bg") //
	{
		if(num_args > 1){
			// too many arguments
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
		}
		else if (num_args == 0 && mp.empty()) {
			// no arguments passed, and map is empty
			std::cerr << "smash error: bg: jobs list is empty" << std::endl;
		}
		else if (num_args == 0 && (find_stopped() == -1)) {
			// no arguments passed, no stopped job
			std::cerr << "smash error: bg: there are no stopped jobs to resume" << std::endl;
		}
		else if (num_args != 0 && !is_number(args[1])){
			// argument is not a number - invalid argument
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
		}
		else if ((num_args != 0) && (arg_in_map(args[1])==-1)) {
			// job id is not found
			std::cerr << "smash error: bg: job-id" << args[1] << " does not exist" << std::endl;
		}
		else {
			// no error
			int job_id = 0;
			if(num_args == 0){
				job_id = find_stopped();
			}
			else job_id = std::stoi(args[1]);
			if (mp[job_id].state == Running){
				// this job is already running
				std::cerr << "smash error: bg: job-id "<< job_id << " is already running in the background" << std::endl
			}
			else {
				std::cout << mp[job_id].cmd << " : " << mp[job_id].pid << std::endl;
				kill(mp[job_id].pid, SIGCONT);
				mp.erase(job_id);
			}
		}
	}
	/*************************************************/
	else if (args[CMD] == "quit")
	{
		pid_t child_pid;
   		if(args[1] == "kill"){

   			for (auto it = mp.begin(); it != mp.end(); ++it){ // for each of the jobs running
   				if(kill(it->first, SIGTERM)){
   					// kill error
   					std::cerr << "smash error: kill SIGTERM error" << std::endl;
   					return 1;
   				}
   				std::cout << "[" << it->first << "] " << it->second.cmd << " - " <<"Sending SIGTERM...";
   				fflush( stdout);
   				sleep(5);
   				child_pid = waitpid(it->second.pid, NULL, WNOHANG);
   				if ( child_pid == -1) {
   					// waitpid error
   					std::cerr << "smash error: waitpid failed";
   					return 1;
   				}
   				if( child_pid != it->second.pid){
   					// the process hasn't finish yet
   					std::cout << " (5 sec passed) Sending SIGKILL…";
   					if (kill(it->second.pid, SIGKILL)){
   						std::cerr << "smash error: kill failed" << std::endl;
   						return 1;
   					}
   				}
   				std::cout << " Done." << std::endl;
			  }
   		}
   		exit(0);
	} 
	/*************************************************/
	else // external command
	{
	 	return ExeExternal(args,num_args, cmdString);
	}
	if (illegal_cmd == true)
	{
		std::cerr << "smash error: " << cmdString << std::endl;
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
int ExeExternal(std::string args[MAX_ARG], int num_args, std::string cmdString)
{
	pid_t pID;
	switch(pID = fork()) {
		case FAILED:
					// Add your code here (error)
    			    std::cerr << "smash error: fork failed" << std::endl;
    			    exit(1);
					/* 
					your code
					*/
		case 0 :
                	// Child Process

			        // Add your code here (execute an external command)
        			setpgrp();
					const char** argv = new const char* [num_args + 2];
					for (int i = 0; i < num_args + 1; i++)
						argv[i] = const_cast<char*>(args[i].c_str());
					argv[num_args + 1] = NULL;
					execv(argv[CMD], argv);
					std::cerr << "smash error: execv failed" << std::endl;
					//waitpid(0, NULL, WCONTINUED);

		default:
                	// Add your code here
					fg_clear();
					fg_insert(pID, cmdString);
					waitpid(pID,NULL,WEXITED );
					/* 
					your code
					*/
	}
	return FAILED;
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
/*int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)

	} 
	return -1;
}*/
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(std::map<int, job, less<int>> &mp, std::string args[MAX_ARG], int num_args, std::string cmdString)
{
	if (num_args > 0 && cmdString[cmdString.length()-1] == '&')//&& is_path_cmd(args[CMD]) ?
	{
		if (args[num_args][args[num_args].length() - 1] != '&') {
			//ERROR
			exit(1)
		}
		args[num_args].pop_back();
		if (args[num_args].empty()) {
			num_args -= 1;
		}
		// Add your code here (execute a in the background)
					
		pid_t pID;
		switch(pID = fork()){
				case FAILED:
						// Add your code here (error)
						std::cerr << "smash error: fork failed" << std::endl;
						exit(1);
						/*
						your code
						*/
				case 0 :
						// Child Process
						// Add your code here (execute an external command)
						setpgrp();
						const char** argv = new const char* [num_args + 2];
						for (int i = 0; i < num_args + 1; i++)
							argv[i] = const_cast<char*>(args[i].c_str());
						argv[num_args + 1] = NULL;
						execv(argv[CMD], argv);
						std::cerr << "smash error: execv failed" << std::endl;
						//waitpid(0, NULL, WCONTINUED);

				default:
						// Add your code here
						//waitpid(pID,NULL,WEXITED );
						/*
						your code
						*/
						if (!addNewJob(pID, cmdString)) {//Need to be discussed about which cmd text we store in map.
							//FAILED! type bool function
						}
						return SUCCESS;
		}
		
	}
	return FAILED;
}


bool is_number(std::string& str)
{
    for (char const &c : str) {
        // using the std::isdigit() function
        if (std::isdigit(c) == 0)
          return false;
    }
    return true;
}
bool is_number_char(char * str)
{
	std::string str_s(str);
    for (char const &c : str_s) {
        // using the std::isdigit() function
        if (std::isdigit(c) == 0)
          return false;
    }
    return true;
}


int arg_in_map(std::string& arg){
	int num = std::stoi(arg);
	if (mp.find(num) == mp.end())
		return FAILED;
	return num;
}

bool addNewJob(pid_t pID, std::string cmd, job_state state = Running) {
	time_t curr_time(time(NULL));
	job newjob(pID, cmd, state, curr_time);
	return mp.insert(std::pair<int, job>(last_job, newjob)).second;
}
