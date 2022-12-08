//		commands.c
//********************************************


#include "commands.h"
#include "signals.h"


int last_job = 0;
char* last_path = NULL;
char* current_path = NULL;
std::map<int, job, std::less<int>> mp;

//FG handling
pid_t fg_pid = -1; //PID of the foreground process. Initialy/not in use, has value of impossible pid.
std::string fg_cmd;
int fg_job_id = -1;

//********************************************
// function name: update_jobs_list
// Description: Updates the job list according to which job has finished (remove
// it from the list) and which job is stopped/running.
// Parameters: none
// Returns: void
//********************************************
void update_jobs_list()
{
	int status;
	pid_t child_pid;
	// Checks which sons are still running
	for (auto it = mp.begin(); it != mp.end(); ++it)
	{
		status = 0;
		child_pid = waitpid((it->second).pid,&status, WNOHANG|WUNTRACED|WCONTINUED);
		if (child_pid == -1) //waitpid failed
		{
			std::perror("smash error: waitpid failed");
			return;
		}

		if(WIFSTOPPED(status))
		{
			(it->second).state = Stopped;

		}
		else if(WIFCONTINUED(status))
		{
			(it->second).state = Running;
		}
		else if(child_pid == (it->second).pid) // the process is not running
		{
			mp.erase(it);
		}
	}
	// Update last_job array to match the last job in jobs (with the biggest job-id)
	if (mp.empty()) {
		last_job = 0;
	}
	else{
		last_job = mp.rbegin()->first;
	}
}

//********************************************
// function name: is_built_in_cmd
// Description:  returns true if the cmd entered is one of the built in
// commands stored in 'BUILT_IN_CMD' array
// Parameters: the cmd as string
// Returns: bool - true if the cmd is built-in command
//********************************************
bool is_built_in_cmd(std::string cmd) {
	// Rm the & whether exists
	if (cmd[cmd.length() - 1] == '&') {
		cmd.pop_back();
		if (cmd.empty()) {
			return false;
		}
	}
	// Search in 'BUILT_IN_CMD' the cmd
	return (std::find(std::begin(BUILT_IN_CMD), std::end(BUILT_IN_CMD), cmd) != std::end(BUILT_IN_CMD));
}

//********************************************
// function name: is_fg_exists
// Description: Returns true if fg command is currently running, false otherwise
//********************************************
bool is_fg_exists() {
	if ((fg_pid != -1) && (!fg_cmd.empty())) {
		return true;
	}
	return false;
}

//********************************************
// function name: is_fg_have_job_id
// Description: Returns true if the fg job that is currently running
// has already got a job-id allocation
//********************************************
bool is_fg_have_job_id() {
	return (is_fg_exists() && fg_job_id != -1);
}

//********************************************
// function name: fg_clear
// Description: Clears the fg job's metadata - meaning no job is currently
// running in the foreground
//********************************************
void fg_clear() {
	fg_pid = -1;
	fg_cmd.clear();
	fg_job_id = -1;
}

//********************************************
// function name: fg_insert
// Description: Inserts new metadata information about the command
// that is currently running in the foreground
// Parameters: newPid - the pid of the job, newCmd - the command of the job,
// job_id - the id of the job.
//********************************************
void fg_insert(pid_t newPid, std::string newCmd, int job_id) {
	fg_pid = newPid;
	fg_cmd = newCmd;
	fg_job_id = job_id;
}


//********************************************
// job:
// The struct of every job in 'mp'(the dynamic std::map that contains all of the
// current jobs).
// Stores the important information about the job - its pid, the cmd of its origin,
// the time it was added to the map and its current state (Running/Stopped)
//********************************************
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
// function name: find_stopped
// Description: Finds the last job which is Stopped in the list of jobs.
// Returns: int of the last stopped job-id if it exists, -1 otherwise.
//********************************************
int find_stopped()
{
	bool exist_one = false;
	int stopped;
	for (auto it = mp.begin(); it != mp.end(); ++it){
		if ((it->second).state == Stopped) {
			stopped = it->first;
			exist_one = true;
		}
	}
	if(!exist_one) return -1;

	return stopped;
}

//********************************************
// function name: cmdline_split_into_arguments
// Description: Splits a given command line input, according to the delimiters, into
// an array of arguments.
// The user has the responsibility of creating and deleting the arguments space/slots.
// Parameters: line - the string of the whole line entered. &args - pointer to an empty
// array of arguments. delimiters - the delimiters which by them the split splits.
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
		// arg[i] is the arguments by order
		iter = strtok(NULL, const_cast<char*>(delimiters.c_str()));
		if (iter != NULL) {
			args[i] = iter;
			num_arg++;  // number of arguments of cmd
		}

	}
	return num_arg;
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//********************************************************
int ExeCmd(std::string args[MAX_ARG], int num_args, std::string cmdString)
{
	
	bool illegal_cmd = false; // illegal command
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/

	if (args[CMD] == "cd")
	{
		// Cd error checks
		if (num_args > 1)  // too many arguments
			std::cerr << "smash error: cd: too many arguments\n";
		else if (args[1] == "-") {
			if (last_path == NULL) {
				 // there is no last path specified yet since the start of the program
				std::cerr << "smash error: cd: OLDPWD not set\n";
				illegal_cmd = true;
			}
			else if (chdir(last_path)) {
				perror("smash error: chdir failed");
				illegal_cmd = true;
			}
			else {
				// In case of successful cd - Update last path by swap current<->last paths
				char* temp_path = current_path;
				current_path = last_path;
				last_path = temp_path;
			}
		}
		else {
			//
			if (chdir(const_cast<char*>(args[1].c_str()))){
				perror("smash error: chdir failed");
			}
			else {
				// In case of successful cd - Update last path
				char* current_path_tmp = get_current_dir_name();
				if (current_path_tmp == NULL) {
					perror("smash error: getcwd failed");
					return FAILED;
				}
				free(last_path);
				last_path = current_path;
				current_path = current_path_tmp;
			}
		}
	} 

	
	/*************************************************/
	else if (args[CMD] == "pwd")
	{
		if (current_path) {
			std::cout << current_path << std::endl;
		}
		else {
			// Update 'current_path' to store the current path
			char* cwd = get_current_dir_name();
			if (cwd == NULL) {
				perror("smash error: getcwd failed");
				return FAILED;
			}
			else { 
				// print the pwd
				std::cout << cwd << std::endl; 
				current_path = cwd;
			}
		}
	}
	
	/*************************************************/
	else if (args[CMD] == "kill")
	{
		// Valid arguments check
		if (num_args != 2) {
			std::cerr << "smash error: kill: invalid arguments\n";
			illegal_cmd = true;
		}
		else{
			std::string signum_s(args[1]);
			std::string job_id_s(args[2]);
			if (signum_s[0] != '-'){ // kill's first argument must start with '-'
				std::cerr << "smash error: kill: invalid arguments\n";
				illegal_cmd = true;
			}
			else signum_s.erase(0,1); // erase the '-' in kill's first arg
			if (!illegal_cmd && (!is_number(signum_s) || !is_number(job_id_s) )){
				// one/both of the kill's arguments are not integers - error
				std::cerr << "smash error: kill: invalid arguments\n";
				illegal_cmd = true;
			}
			else if(!illegal_cmd){
				// job_id check
				int job_id = std::stoi(job_id_s);
				int signum = std::stoi(signum_s);
				// Search the job-id in the jobs map
				if(mp.find(job_id) == mp.end()){
					// The job-id entered is not found in the jobs map - error
					std::cerr << "smash error: kill: job-id " <<job_id<<" does not exist" << std::endl;
					illegal_cmd = true;
				}
				else {
					if (kill(mp[job_id].pid , signum)){
						std::perror("smash error: kill failed");
						illegal_cmd = true;
					}
					// Signal successfully sent
					std::cout << "signal number "<< signum<< " was sent to pid "<<mp[job_id].pid << std::endl;
				}
			}
		}
	}
	else if (args[CMD] == "diff")
	{
		// Valid arguments check
		if (num_args != 2) {
			std::cerr << "smash error: diff: invalid arguments\n";
			illegal_cmd = true;
		}
		else {
			FILE *f1, *f2;
			f1 = fopen(const_cast<char*>(args[1].c_str()),"r");
			if( f1 == NULL){
				std::perror("smash error: open failed");
				return FAILED;
			}
			f2 = fopen(const_cast<char*>(args[2].c_str()),"r");
			if (f2 == NULL){
				std::perror("smash error: open failed");
				return FAILED;
			}
			unsigned char c1 = 0;
			unsigned char c2 = 0;
			bool diff = 0;
			// Checks every char in the file if its the same
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
	
	else if (args[CMD] == "jobs")
	{
		// Update the state(Running/Stopped) of every job in the jobs map
		update_jobs_list();
		time_t curr_time(time(NULL));
		if (curr_time == -1) {
			std::perror("smash error: time failed");
			return FAILED;
		}
		double diff_time;
		for (auto it = mp.begin(); it != mp.end(); ++it){
			// Prints the details of every job in the jobs map
			 diff_time = difftime(curr_time, (it->second).entered_time);
			 std::cout << "[" << it->first << "] " << (it->second).cmd
					 << " : " << (it->second).pid << " "
					 << diff_time << " secs";
			 if((it->second).state == Stopped){
				 std::cout << " (stopped)";
			 }
			 std::cout << std::endl;
		}
	}
	/*************************************************/
	else if (args[CMD] == "showpid")
	{
		std::cout << "smash pid is " << getpid() << std::endl;
	}
	/*************************************************/
	else if (args[CMD] == "fg")
	{
		if(num_args > 1){
			// too many arguments - error
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
			illegal_cmd = true;
		}
		else if (num_args == 0 && mp.empty()) {
			// no arguments passed, and map is empty - error
			std::cerr << "smash error: fg: jobs list is empty" << std::endl;
		}
		else if (num_args != 0 && !is_number(args[1])){
			// argument is not a number - invalid argument - error
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
		}
		else if ((num_args != 0) && (arg_in_map(args[1])==-1)) {
			// job id is not found in the jobs map - error
			std::cerr << "smash error: fg: job-id " << args[1] << " does not exist" << std::endl;
		} else {
			// no error
			int job_id = -1;
			if (num_args == 0) { // take the biggest job id
				job_id = mp.rbegin()->first; // the biggest key
			}
			else job_id = arg_in_map(args[1]); // take the arg job_id
			// Update which job is currently in foreground state
			fg_clear();
			fg_insert(mp[job_id].pid, mp[job_id].cmd, job_id);
			std::cout << mp[job_id].cmd << " : " << mp[job_id].pid << std::endl;
			mp.erase(job_id);

			if (kill(fg_pid, SIGCONT)) {
				std::perror("smash error: kill failed");
				return FAILED;
			}
			// wait for job to finish - running in foreground
			if (waitpid(fg_pid, NULL, WUNTRACED) == -1) {
				std::perror("smash error: waitpid failed");
				return FAILED;
			}
			// after wait_pid finishes - the job no longer runs in the foreground
			fg_clear();
		}
	} 
	/*************************************************/
	else if (args[CMD] == "bg") //
	{
		if(num_args > 1){
			// too many arguments - error
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
			illegal_cmd = true;
		}
		else if (num_args == 0 && (find_stopped() == -1)) {
			// no arguments passed, no stopped job in jobs map - error
			std::cerr << "smash error: bg: there are no stopped jobs to resume" << std::endl;
			illegal_cmd = true;
		}
		else if (num_args != 0 && !is_number(args[1])){
			// argument is not a number - invalid argument - error
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
			illegal_cmd = true;
		}
		else if ((num_args != 0) && (arg_in_map(args[1])==-1)) {
			// job id is not found in the jobs map - error
			std::cerr << "smash error: bg: job-id " << args[1] << " does not exist" << std::endl;
			illegal_cmd = true;
		}
		else {
			int job_id = 0;
			// job_id is the job in arg[1] and if it wasn't entered by the user its the
			// last stopped job in the jobs map
			if(num_args == 0){
				job_id = find_stopped(); // last stopped job
			}
			else {
				job_id = std::stoi(args[1]); // entered job
			}
			if (mp[job_id].state == Running){
				// this job is already running - error
				std::cerr << "smash error: bg: job-id " << job_id << " is already running in the background" << std::endl;
			}
			else {
				std::cout << mp[job_id].cmd << " : " << mp[job_id].pid << std::endl;
				if (kill(mp[job_id].pid, SIGCONT)) {
					std::perror("smash error: kill failed");
					return FAILED;
				}
				mp[job_id].state = Running;
			}
		}
	}
	/*************************************************/
	else if (args[CMD] == "quit")
	{
		pid_t child_pid;
   		if(args[1] == "kill"){
   			// kill every job in jobs map. at first send SIGTERM signal,
   			// if after 5 seconds it wasn't killed - send SIGKILL signal
   			for (auto it = mp.begin(); it != mp.end(); ++it){ // for each of the jobs running
				std::cout << "[" << it->first << "] " << it->second.cmd << " - " <<"Sending SIGTERM...";
   				if(kill(it->second.pid, SIGTERM)){
   					// kill error
   					std::perror("smash error: kill failed");
   					return FAILED;
   				}
   				
   				fflush( stdout);
   				// wait 5 seconds
   				sleep(5);
   				// Check if the process was killed by SIGTERN signal
   				child_pid = waitpid(it->second.pid, NULL, WNOHANG);
   				if ( child_pid == -1) {
   					// waitpid error
   					std::perror("smash error: waitpid failed");
   					return FAILED;
   				}
   				if( child_pid != it->second.pid){
   					// the process wasn't killed yet - send SIGKILL
   					std::cout << " (5 sec passed) Sending SIGKILL…";
   					if (kill(it->second.pid, SIGKILL)){
   						std::perror("smash error: kill failed");
   						return FAILED;
   					}
   				}
   				std::cout << " Done." << std::endl;
			  }
   		}
		free(last_path);
		free(current_path);
   		exit(0);
	} 
	/*************************************************/
	if (illegal_cmd == true)
	{
		return FAILED;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, number of actual arguments, and external command string.
// Returns: Success or Failed
//**************************************************************************************
int ExeExternal(std::string args[MAX_ARG], int num_args, std::string cmdString)
{
	pid_t pID;
	switch(pID = fork()) {
		case FAILED:
					// fork failed error
					std::perror("smash error: fork failed");
					free(last_path);
					free(current_path);
    			    exit(1);

		case 0 :
                	// Child Process
					char* argv[MAX_ARG];
					// Arrange the args in argv[] for execv()
					for (int i = 0; i < num_args + 1; i++)
						argv[i] = const_cast<char*>(args[i].c_str());
					argv[num_args + 1] = NULL;
					setpgrp();
					execv(argv[CMD], argv);
					// if we got here it means the execv failed
					std::perror("smash error: execv failed");
					exit(1);

		default:
                	// Parent process - meaning the external cmd was sent to be executed
					fg_clear();
					fg_insert(pID, cmdString);
					// wait for the command to finish in foreground
					if (waitpid(pID, NULL, WUNTRACED) == -1) {
						std::perror("smash error: waitpid failed");
						return FAILED;
					}
					// the command finished
					fg_clear();
					return SUCCESS;
	}
	return FAILED;
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: external background command arguments, number of actual arguments, and external background command string.
// Returns: Success or Failed
//**************************************************************************************
int BgCmd(std::string args[MAX_ARG], int num_args, std::string cmdString)
{		
	pid_t pID;
	switch(pID = fork()){
			case FAILED:
					std::perror("smash error: fork failed");
					free(last_path);
					free(current_path);
					exit(1);

			case 0 :
					// Child Process
					setpgrp();
					char * argv[MAX_ARG];
					for (int i = 0; i < num_args + 1; i++)
						argv[i] = const_cast<char*>(args[i].c_str());
					argv[num_args + 1] = NULL;
					execv(argv[CMD], argv);
					std::perror("smash error: execv failed");
					exit(1);

			default:
					// Parent process - meaning the external cmd was sent to be executed
					if (!addNewJob(pID, cmdString)) {
						return FAILED;
					}
					return SUCCESS;
	}
	return FAILED;
}


//********************************************
// function name: is_number
// Returns: bool 'true' if the string entered is a number
//********************************************
bool is_number(std::string& str)
{
    for (char const &c : str) {
        // using the std::isdigit() function
        if (std::isdigit(c) == 0)
          return false;
    }
    return true;
}

//********************************************
// function name: is_number_char
// Returns: bool 'true' if the char entered is a number
//********************************************
bool is_number_char(char * str)
{
	// cnvert the char to a string
	std::string str_s(str);
    for (char const &c : str_s) {
        // using the std::isdigit() function
        if (std::isdigit(c) == 0)
          return false;
    }
    return true;
}

//********************************************
// function name: arg_in_map
// Description: Searches whether the job-id entered( in string format)
// is in the jobs map. It is called after making sure this arg is a valid number.
// Returns: the int job-id if it was found, -1 otherwise
//********************************************
int arg_in_map(std::string& arg){
	int num = std::stoi(arg);
	if (mp.find(num) == mp.end())
		return FAILED;
	return num;
}

/*
	addNewJob:
		By a given process, whether it has already a job ID or not.
		The function will insert the process as a new job object into the jobs ADT.
		returns: boolean state, if the operation succeeded.
*/
bool addNewJob(pid_t pID, std::string cmd, job_state state, int job_id) {
	update_jobs_list();
	time_t curr_time(time(NULL));
	if (curr_time == -1) {
		std::perror("smash error: time failed");
		return false;
	}
	job newjob(pID, cmd, state, curr_time);
	int final_job_id = last_job + 1;
	if (job_id != -1) {
		final_job_id = job_id;
	}
	bool result = mp.insert(std::pair<int, job>(final_job_id, newjob)).second;
	return result;
}
