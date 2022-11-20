//		commands.c
//********************************************
#include <iostream>
#include "commands.h"
//using namespace std;

//using std::cout;
//using std::endl;
//using std::string;

class job{
public:
	int pid;
	std::string cmd;
	job_state state;
	time_t entered_time; // the time the job entered the list
	job(int pida,std::string cmda,job_state statea,time_t entered_timea);
	~job();
};
job::job(int pida,std::string cmda,job_state statea,time_t entered_timea)
{
		pid=pida;
		cmd=cmda;
		state=statea;
		entered_time=entered_timea;
}
job::~job(){}


//********************************************
int find_stopped()
{
	for (auto it = mp.begin(); it != mp.end(); ++it)
		if ((it->second).state == Stopped)
			return it->first;
	return -1;
}
//********************************************

// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd; // arg[0] is the cmd
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); // arg[i] is the arguments by order
		if (args[i] != NULL) 
			num_arg++;  // number of arguments of cmd
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) //
	{
		if( num_arg > 1) perror("smash error: cd: too many arguments");

		else if (args[1] == '-'){
			if ( last_path == NULL) perror("smash error: cd: OLDPWD not set"); // TBD initialize to NULL
			else if(chdir(last_path)){
				perror("chdir() error");
			}
		}
		else {
			if(chdir(arg[1])) perror("chdir() error");
		}

		char * last_path_tmp = get_current_dir_name();
		if ( last_path_tmp == NULL)
			perror("getcwd() error");
		free(last_path);
		last_path = last_path_tmp;
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd"))  //
	{
		char * cwd = get_current_dir_name();
		if ( cwd == NULL)
			perror("getcwd() error"); // TBD
		else
		    printf("%s", *cwd);
		free(cwd);
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
 		
	}
	else if (!strcmp(cmd, "diff"))
	{

	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) //
	{
		time_t * curr_time = new time_t;;
		double diff_time;
		time(curr_time);
		delete curr_time;

		//for (auto it : mp){

		for (auto it = mp.begin(); it != mp.end(); ++it){
			 diff_time = diff_time(*curr_time, (it->second).entered_time);
			 std::cout << "[" << it->first << "] " << (it->second).cmd
					 << " : " << (it->second).pid << " "
					 << diff_time << " (" << (it->second).state << ")"
					 << std::endl ;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))  //
	{
		std::cout << "smash pid is " << getpid() << std::endl;
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))  //
	{
		if((num_arg > 1) || (num_arg > 0 && decltype(args[1]) != int) ) {
			// wrong type or format - need to check!!!
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
		}
		else if (num_arg == 0 && mp.empty()) {
			// no arguments passed, and map is empty
			std::cerr << "smash error: fg: jobs list is empty" << std::endl;
		}
		else if (mp.find((int)args[1]) == mp.end()) {
		  // not found
			std::cerr << "smash error: fg: job-id" << args[1] << " does not exist" << std::endl;
		} else {
			// found
			std::cout <<  mp[(int)args[1]].cmd << " : " << mp[(int)args[1]].pid << std::endl;
			kill(mp[(int)args[1]].pid, SIGCONT );
			waitpid(mp[(int)args[1]].pid, NULL, WCONTINUED);
			mp.erase((int)args[1]);
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) //
	{
		if((num_arg > 1) || (num_arg > 0 && decltype(args[1]) != int) ) {
			// wrong type or format - need to check!!!
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
		}
		else if (num_arg == 0 && mp.empty()) {
			// no arguments passed, and map is empty
			std::cerr << "smash error: fg: jobs list is empty" << std::endl;
		}
		else if (num_arg == 0 && (find_stopped() == -1)) {
			// no arguments passed, no stopped job
			std::cerr << "smash error: bg: there are no stopped jobs to resume" << std::endl;
		}
		else if (mp.find((int)args[1]) == mp.end()) {
		  // not found
			std::cerr << "smash error: fg: job-id" << args[1] << " does not exist" << std::endl;
		} else {
			// found
			if (mp[(int)args[1]].state == Running){
				// this job is already running
				std::cerr << "smash error: bg: job-id "<< args[1] << " is already running in the background" << std::endl
			}
			else {
				std::cout <<  mp[(int)args[1]].cmd << " : " << mp[(int)args[1]].pid << std::endl;
				kill(mp[(int)args[1]].pid, SIGCONT );
				mp.erase((int)args[1]);
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit")) //
	{
   		
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
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
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
    			    std::cerr << 'smash error: fork failed' << std::endl;
    			    exit(1);
					/* 
					your code
					*/
        	case 0 :
                	// Child Process

			        // Add your code here (execute an external command)
        			setpgrp();
					execv(args[0], args+1);
					std::cerr << 'smash error: execv failed' << std::endl;
					//waitpid(0, NULL, WCONTINUED);

			default:
                	// Add your code here
					//execv(args[0], args+1);
					waitpid(pID,NULL,WEXITED );
					/* 
					your code
					*/
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)

	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{
	char* cmd;
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
		cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;

	args[0] = cmd; // arg[0] is the cmd
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); // arg[i] is the arguments by order
		if (args[i] != NULL)
			num_arg++;  // number of arguments of cmd

	}
	//char* Command;
	//char* delimiters = " \t\n";
	//char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
					
		int pID;
		    	switch(pID = fork())
			{
				case -1:
						// Add your code here (error)
						std::cerr << 'smash error: fork failed' << std::endl;
						exit(1);
						/*
						your code
						*/
				case 0 :
						// Child Process
						time_t * curr_time = new time_t;
						time(curr_time);
						job newjob(pID,std::string cmd_s(cmd),Running,*curr_time);
						delete curr_time;
						// Add your code here (execute an external command)
						setpgrp();
						execv(args[0], args+1);
						std::cerr << 'smash error: execv failed' << std::endl;
						//waitpid(0, NULL, WCONTINUED);

				default:
						// Add your code here
						//execv(args[0], args+1);
						//waitpid(pID,NULL,WEXITED );
						/*
						your code
						*/
			}
		
	}
	return -1;
}

