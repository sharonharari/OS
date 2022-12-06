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

void update_jobs_list()
{
	int status;
	pid_t child_pid;
	// checking which sons are still running
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
	if (mp.empty()) {
		last_job = 0;
	}
	else{
		last_job = mp.rbegin()->first;
	}
}

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
	return false;
}
bool is_fg_have_job_id() {
	return (is_fg_exists() && fg_job_id != -1);
}
void fg_clear() {
	fg_pid = -1;
	fg_cmd.clear();
	fg_job_id = -1;
}
void fg_insert(pid_t newPid, std::string newCmd, int job_id) {
	fg_pid = newPid;
	fg_cmd = newCmd;
	fg_job_id = job_id;
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
			std::cerr << "smash error: cd: too many arguments\n";
		else if (args[1] == "-") {
			if (last_path == NULL) {
				std::cerr << "smash error: cd: OLDPWD not set\n"; // TBD initialize to NULL
				illegal_cmd = true;
			}
			else if (chdir(last_path)) {
				perror("smash error: chdir failed");
				illegal_cmd = true;
			}
			else {
				char* temp_path = current_path;
				current_path = last_path;
				last_path = temp_path;
			}
		}
		else {
			if (chdir(const_cast<char*>(args[1].c_str()))) perror("smash error: chdir failed");
			else {
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
	else if (args[CMD] == "pwd")  //
	{
		if (current_path) {
			std::cout << current_path << std::endl;
		}
		else {
			char* cwd = get_current_dir_name();
			if (cwd == NULL) {
				perror("smash error: getcwd failed");
				return FAILED;
			}
			else { 
				std::cout << cwd << std::endl; 
				current_path = cwd;
			}
		}
	}
	
	/*************************************************/
	else if (args[CMD] == "kill")
	{
		// valid arguments check
		if (num_args != 2) {
			std::cerr << "smash error: kill: invalid arguments\n";
			illegal_cmd = true;
		}
		else{
			std::string signum_s(args[1]);
			std::string job_id_s(args[2]);
			if (signum_s[0] != '-'){
				std::cerr << "smash error: kill: invalid arguments\n";
				illegal_cmd = true;
			}
			else signum_s.erase(0,1);
			if (!is_number(signum_s) || !is_number(job_id_s) ){
				std::cerr << "smash error: kill: invalid arguments\n";
				illegal_cmd = true;
			}
			else if(!illegal_cmd){
				// job_id check
				int job_id = std::stoi(job_id_s);
				int signum = std::stoi(signum_s);
				if(mp.find(job_id) == mp.end()){
					std::cerr << "smash error: kill: job-id " <<job_id<<" does not exist" << std::endl;
					illegal_cmd = true;
				}
				else {
					if (kill(mp[job_id].pid , signum)){
						std::perror("smash error: kill failed");
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
		update_jobs_list();
		time_t curr_time(time(NULL));
		double diff_time;
		for (auto it = mp.begin(); it != mp.end(); ++it){
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
			std::cerr << "smash error: fg: job-id " << args[1] << " does not exist" << std::endl;
		} else {
			// no error

			int job_id = -1;
			if (num_args == 0) { // take the biggest job id
				job_id = mp.rbegin()->first; // the biggest key
			}
			else job_id = arg_in_map(args[1]); // take the arg job_id
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
			fg_clear();
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
			std::cerr << "smash error: bg: job-id " << args[1] << " does not exist" << std::endl;
			illegal_cmd = true;
		}
		else {
			// no error
			int job_id = 0;
			if(num_args == 0){
				job_id = find_stopped();
			}
			else {
				job_id = std::stoi(args[1]);
			}
			if (mp[job_id].state == Running){
				// this job is already running
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

   			for (auto it = mp.begin(); it != mp.end(); ++it){ // for each of the jobs running
				std::cout << "[" << it->first << "] " << it->second.cmd << " - " <<"Sending SIGTERM...";
   				if(kill(it->second.pid, SIGTERM)){
   					// kill error
   					std::perror("smash error: kill failed");
   					return FAILED;
   				}
   				
   				fflush( stdout);
   				sleep(5);
   				child_pid = waitpid(it->second.pid, NULL, WNOHANG);
   				if ( child_pid == -1) {
   					// waitpid error
   					std::perror("smash error: waitpid failed");
   					return FAILED;
   				}
   				if( child_pid != it->second.pid){
   					// the process hasn't finish yet
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
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
int ExeExternal(std::string args[MAX_ARG], int num_args, std::string cmdString)
{
	pid_t pID;
	switch(pID = fork()) {
		case FAILED:
					std::perror("smash error: fork failed");
					free(last_path);
					free(current_path);
    			    exit(1);

		case 0 :
                	// Child Process
					char* argv[MAX_ARG];
					for (int i = 0; i < num_args + 1; i++)
						argv[i] = const_cast<char*>(args[i].c_str());
					argv[num_args + 1] = NULL;
					setpgrp();
					execv(argv[CMD], argv);
					std::perror("smash error: execv failed");
					exit(1);

		default:
                	// Add your code here
					fg_clear();
					fg_insert(pID, cmdString);
					if (waitpid(pID, NULL, WUNTRACED) == -1) {
						std::perror("smash error: waitpid failed");
						return FAILED;
					}
					fg_clear();
					return SUCCESS;
	}
	return FAILED;
}

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
					if (!addNewJob(pID, cmdString)) {
						return FAILED;
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

bool addNewJob(pid_t pID, std::string cmd, job_state state, int job_id) {
	update_jobs_list();
	time_t curr_time(time(NULL));
	job newjob(pID, cmd, state, curr_time);
	int final_job_id = last_job + 1;
	if (job_id != -1) {
		final_job_id = job_id;
	}
	bool result = mp.insert(std::pair<int, job>(final_job_id, newjob)).second;
	return result;
}
