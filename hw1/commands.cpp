//		commands.c
//********************************************

#include <iostream>
#include "commands.h"
#include <map>
#include <algorithm>    // std::find
#include <cerrno>
#include <cstdio>

int last_job = 0;
char* last_path = NULL;
std::map<int, job, std::less<int>> mp;


bool is_built_in_cmd(std::string cmd) {
	if (cmd[cmd.length() - 1] == '&') {
		cmd.pop_back();
		if (cmd.empty()) {
			return false;
		}
	}
	return (std::find(std::begin(BUILT_IN_CMD), std::end(BUILT_IN_CMD), cmd) != std::end(BUILT_IN_CMD));
}



bool is_fg_exists() {
	if ((fg_pid != -1) && (!fg_cmd.empty())) {
		return true;
	}
	//std::cerr << "fg container flawed/broken." << std::endl;
	return false;
}
void fg_clear() {
	fg_pid = -1;
	fg_cmd.clear();
}
void fg_insert(pid_t newPid, std::string newCmd) {
	fg_pid = newPid;
	fg_cmd = newCmd;
}

job::job():pid(-1),cmd(""),state(Stopped),entered_time(time(NULL)){}
job::job(pid_t pida,std::string cmda,job_state statea,time_t entered_timea)
{
		pid=pida;
		cmd=cmda;
		state=statea;
		entered_time=entered_timea;
}
job::job(const job& new_job) {
	pid = new_job.pid;
	cmd = new_job.cmd;
	state = new_job.state;
	entered_time = new_job.entered_time;
}
job& job::operator=(const job& new_job) {
	if (this == &new_job) {
		return *this;
	}
	pid = new_job.pid;
	cmd = new_job.cmd;
	state = new_job.state;
	entered_time = new_job.entered_time;
	return *this;
}
job::~job(){}




//********************************************
int find_stopped()
{
	bool exist_one = false;
	job stopped;
	for (auto it = mp.begin(); it != mp.end(); ++it)
		if ((it->second).state == Stopped) {
			stopped = it->second;
			exist_one = true;
		}
	if(!exist_one) return -1;

	return stopped.pid;
}
//********************************************


int cmdline_split_into_arguments(std::string line, std::string(&args)[MAX_ARG], std::string delimiters)
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
int ExeCmd(std::string args[MAX_ARG], int num_args, std::string cmdString)
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
			if (last_path == NULL) {
				perror("smash error: cd: OLDPWD not set"); // TBD initialize to NULL
				illegal_cmd = true;
			}
			else if (chdir(last_path)) {
				perror("chdir() error");
				illegal_cmd = true;
			}
		}
		else {
			if (chdir(const_cast<char*>(args[1].c_str()))) perror("chdir() error");
			else {
				char* last_path_tmp = get_current_dir_name();
				if (last_path_tmp == NULL) {
					perror("getcwd() error");
					return FAILED;
				}
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
		if (cwd == NULL) {
			perror("smash error: getcwd() error");
			return FAILED;
		}
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
			illegal_cmd = true;
		}
		else{
			std::string signum_s(args[1]);
			std::string job_id_s(args[2]);
			if (signum_s[0] != '-'){
				perror("smash error: kill: invalid arguments");
				illegal_cmd = true;
			}
			else signum_s.erase(0,1);
			if (!is_number(signum_s) || !is_number(job_id_s) ){
				perror("smash error: kill: invalid arguments");
				illegal_cmd = true;
			}
			else {
				// job_id check
				int job_id = std::stoi(job_id_s);
				int signum = std::stoi(signum_s);
				if(mp.find(job_id) == mp.end()){
					std::cerr << "smash error: kill: job-id " <<job_id<<" does not exist" << std::endl;
					illegal_cmd = true;
				}
				else {
					if (kill(mp[job_id].pid , signum)){
						std::cout <<"smash error: kill: job-id " << job_id << " - Cannot send signal\n";
						illegal_cmd = true;
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
			illegal_cmd = true;
		}
		else {
			FILE *f1, *f2;
			f1 = fopen(const_cast<char*>(args[1].c_str()),"r");
			if( f1 == NULL){
				std::cerr << "smash error : failed to open file" << std::endl;
				return FAILED;
			}
			f2 = fopen(const_cast<char*>(args[2].c_str()),"r");
			if (f2 == NULL){
				std::cerr << "smash error : failed to open file" << std::endl;
				return FAILED;
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
			illegal_cmd = true;
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

			if (kill(mp[job_id].pid, SIGCONT)) {
				std::perror("smash error: kill failed");
				return FAILED;
			}
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
			illegal_cmd = true;
		}
		else if (num_args == 0 && mp.empty()) {
			// no arguments passed, and map is empty
			std::cerr << "smash error: bg: jobs list is empty" << std::endl;
			illegal_cmd = true;
		}
		else if (num_args == 0 && (find_stopped() == -1)) {
			// no arguments passed, no stopped job
			std::cerr << "smash error: bg: there are no stopped jobs to resume" << std::endl;
			illegal_cmd = true;
		}
		else if (num_args != 0 && !is_number(args[1])){
			// argument is not a number - invalid argument
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
			illegal_cmd = true;
		}
		else if ((num_args != 0) && (arg_in_map(args[1])==-1)) {
			// job id is not found
			std::cerr << "smash error: bg: job-id" << args[1] << " does not exist" << std::endl;
			illegal_cmd = true;
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
				std::cerr << "smash error: bg: job-id " << job_id << " is already running in the background" << std::endl;
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
   					return FAILED;
   				}
   				std::cout << "[" << it->first << "] " << it->second.cmd << " - " <<"Sending SIGTERM...";
   				fflush( stdout);
   				sleep(5);
   				child_pid = waitpid(it->second.pid, NULL, WNOHANG);
   				if ( child_pid == -1) {
   					// waitpid error
   					std::cerr << "smash error: waitpid failed";
   					return FAILED;
   				}
   				if( child_pid != it->second.pid){
   					// the process hasn't finish yet
   					std::cout << " (5 sec passed) Sending SIGKILL…";
   					if (kill(it->second.pid, SIGKILL)){
   						std::cerr << "smash error: kill failed" << std::endl;
   						return FAILED;
   					}
   				}
   				std::cout << " Done." << std::endl;
			  }
   		}
   		exit(0);
	} 
	/*************************************************/
	if (illegal_cmd == true)
	{
		//std::cerr << "smash error: " << cmdString << std::endl;
		return FAILED;
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
					char* argv[MAX_ARG];
					for (int i = 0; i < num_args + 1; i++)
						argv[i] = const_cast<char*>(args[i].c_str());
					argv[num_args + 1] = NULL;
					execv(argv[CMD], argv);
					std::cerr << "smash error: execv failed\n" << std::endl;
					//waitpid(0, NULL, WCONTINUED);

		default:
                	// Add your code here
					fg_clear();
					fg_insert(pID, cmdString);
					if (waitpid(pID, NULL, WUNTRACED)) { //Need to be check for different Wparameter.
						std::perror("smash error: waitpid failed\n");
						return FAILED;
					}
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
int BgCmd(std::string args[MAX_ARG], int num_args, std::string cmdString)
{		
	pid_t pID;
	switch(pID = fork()){
			case FAILED:
					// Add your code here (error)
					std::cerr << "smash error: fork failed" << std::endl;
					exit(1);
					/*
						Maybe return?
					*/
			case 0 :
					// Child Process
					// Add your code here (execute an external command)
					setpgrp();
					char * argv[MAX_ARG];
					for (int i = 0; i < num_args + 1; i++)
						argv[i] = const_cast<char*>(args[i].c_str());
					argv[num_args + 1] = NULL;
					execv(argv[CMD], argv);
					std::cerr << "smash error: execv failed" << std::endl;
					return FAILED;

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

bool addNewJob(pid_t pID, std::string cmd, job_state state) {
	time_t curr_time(time(NULL));
	job newjob(pID, cmd, state, curr_time);
	bool result = mp.insert(std::pair<int, job>(last_job, newjob)).second;
	if (result) {
		last_job++;
	}
	return result;
}
