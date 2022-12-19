//		commands.c
//********************************************
#include "commands.h"
//#include "jobs.h"
//********************************************
using namespace std;


char* curr_path;
char prev_path[MAX_LINE_SIZE]="";
list <char*> history_cmds;
char pwd[MAX_LINE_SIZE];


// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(jobs_table* jobs, char* lineSize, char* cmdString)
{
	char* cmd;
	char* args[MAX_ARG];
	const char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;

	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
    getcwd(pwd,sizeof(pwd));

/*************************************************/
    if(strcmp(cmd, "history") || ((!strcmp(cmd,"history"))&&num_arg>0))
    {
        if (history_cmds.size() == MAX_HISTORY_CMDS)
        {
            history_cmds.erase(history_cmds.begin());
        }
		char* tmp = (char*)malloc((strlen(cmdString)+1)*sizeof(char));
		if(tmp != NULL){
			strcpy(tmp, cmdString);
			history_cmds.push_back(tmp);
		}
    }
    if(!strcmp(cmd, "kill"))
    {
        if(num_arg != 2 || ((args[1])[0] != '-'))
        {
            illegal_cmd=TRUE;
        }
        else
        {
            int result_job = jobs->get_job_PID(args[2]);
            if(result_job==-1)
            {
                cout<<"smash error: > kill job -"<<args[2]<<" - job not exist"<<endl;
                return 1;
            }
            int signum = atoi(args[1]+1);
            if(kill(result_job,signum) != 0)
            {
                cout<<"smash error: > kill "<<args[2]<<" - cannot send signal"<<endl;
                return 1;
            }
            cout << "the signal " << strsignal(signum) << " was sent to following pid " << result_job << endl;
        }
    }
    else if(!(strcmp(cmd, "history")))
    {
        if(num_arg==0)
        {
            for (std::list<char*>::iterator it = history_cmds.begin(); it != history_cmds.end(); ++it)
            {
                cout << *it << endl;
            }
            if (history_cmds.size() == MAX_HISTORY_CMDS)
            {
                history_cmds.erase(history_cmds.begin());
            }
		    char* tmp = (char*)malloc((strlen(cmdString)+1)*sizeof(char));
		    if(tmp != NULL)
            {
			    strcpy(tmp, cmdString);
			    history_cmds.push_back(tmp);
		    }
        }
        else
        {
            illegal_cmd=TRUE;
        }
    }
	else if (!strcmp(cmd, "cd") )
	{
        if(num_arg != 1)
            illegal_cmd=TRUE;
        else
        {
            if(!strcmp(args[1],"-")) 
            {
                if(!strcmp(prev_path,""))
                {
                    std::cerr<<"smash error: >"<<"\""<<prev_path<<"\""<<" - path not found"<<std::endl;
                    illegal_cmd=TRUE;
                }
                else
                {
                    if(chdir(prev_path)!=0)
                    {
                        std::cerr<<"smash error: >"<<"\""<<prev_path<<"\""<<" - path not found"<<std::endl;
                        illegal_cmd=TRUE;
                    }
                    else
                    {
                        cout << prev_path << endl;
                        strcpy(prev_path, pwd);
                    }
                }
            }
            else
            {
                if(chdir(args[1])!=0)
                {
                    std::cerr<<"smash error: >"<<"\""<<prev_path<<"\""<<" - path not found"<<std::endl;
                    illegal_cmd=TRUE;
                }
                else
                {
                    strcpy(prev_path,pwd);
                }
            }
        }
	}

	/*************************************************/
	else if (!strcmp(cmd, "pwd"))
	{
	    if(!num_arg)
        {
            cout << pwd << endl;
        }
        else
        {
            illegal_cmd = TRUE;
        }
	}
/*************************************************/

	else if (!strcmp(cmd, "jobs"))
	{
		if (num_arg == 0)
        {
			jobs->print_jobs();
		    cout<<endl;
        }
		else 
        {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{
		if (num_arg == 0) 
        {
			cout << "smash pid is " <<  getpid() << endl;
		}
		else
        {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
	    if(num_arg>1 || ((num_arg==1)&&((atoi(args[1])==0)&&(strcmp(args[1],"0")))))
        {
            illegal_cmd = TRUE;
        }
        else
        {
            // Got no arguement 
            
            if(num_arg == 0) 
            {
                int last_job_pid = jobs->get_recent_PID();
                char* last_job_name = jobs->get_recent_job_name();
                if(last_job_pid == -1 || last_job_name == NULL)
                {
                    cout << "smash error: fg -there is no job in jobs" << endl;
                    return 1;
                }
                cout << last_job_name << endl;
                if(kill(last_job_pid,SIGCONT) == -1) // SIGCONT is 18
                {
                    perror("smash error");
                    return 1;
                }
                fg_pid = last_job_pid;
                fg_name = last_job_name;
                waitpid(last_job_pid,NULL,WUNTRACED);
            }
            else
            {
                int result_job = jobs->get_job_PID(args[1]);
                char* job_name = jobs->get_PID_name(args[1]);
                if(result_job == -1 || job_name == NULL)
                {
                    cout << "smash error: fg -there is no job in jobs" << endl;
                    return 1;
                }
                if(kill(result_job,SIGCONT) == -1) // SIGCONT is 18
                {
                    perror("smash error");
                    return 1;
                }
                fg_name = job_name;
                fg_pid = result_job;
                waitpid(result_job,NULL,0);
                fg_pid = -1;
                exists_in_jobs_table = false;
            }
        }
    }
	
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		if (num_arg > 1 )
        {
			cout << "smash error: > \"More than one arguement for bg.\""<< endl;
			illegal_cmd = TRUE;
		}
		else if ( num_arg == 1)
        {
            int job_idx = atoi(args[1]);
			if(job_idx == 0 && (strcmp(args[1],"0")))
            {
				illegal_cmd = TRUE;
			}
			else { 
                int job_pid = jobs->get_job_PID(args[1]);
				char* job_name = jobs->get_PID_name(args[1]);
				if(job_pid == -1 || job_name == NULL){
					cout << "smash error: bg " << args[1] << "-job does not exist" << endl;
					return 1;
				}
				if (jobs->get_stopped_status(args[1])){
					cout<< job_name << endl;
					if (kill(job_pid,SIGCONT) == -1)
					{
						perror("smash error > failed to continue required process");
						return 1;
					}
				}
				else
				{
					cout << "smash error: bg " << args[1] << "-job is running in background" << endl;
				}
			}
		}

		else 
		{
			int recent_pid = jobs->get_recent_stopped_PID();
			char* recent_name = jobs->get_recent_stopped_name();
			if ( recent_pid == -1 || recent_name == NULL){
				cout << "smash error : bg there are no stopped jobs in the jobs table" << endl;
                return 1;
			}

			cout << recent_name << endl;
			if (kill(recent_pid,SIGCONT) == -1) 
					{
						perror("smash error > failed to continue required process");
						return 1;
					}
		}

	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
        if(num_arg>1)
        {
            illegal_cmd=TRUE;
        }
        else
        {
            if(num_arg==0)
            {
                exit(0);
            }
            else
            {
                if(!(strcmp(args[1],"kill")))
                {
                    list<job>::iterator iter;
                    for (iter = jobs->jobs_list.begin(); iter != jobs->jobs_list.end(); ++iter)
                    {
                        iter->update_job_status();
                        cout << "signal : "  << strsignal(15) << " sent to process : " << iter->job_pid<<endl;
                        if(kill(iter->job_pid,15)) //15 is SIGTERM
                        {
                            perror("smash error");
                            return 1;
                        }
                        else
                        {
                            sleep(5);
                            iter->update_job_status();
                            if(!iter->job_finished)
                            {
                                if(kill(iter->job_pid, 9) != 0)
                                {
                                    perror("smash error");
                                    return 1;
                                }
                            }
                        }
                    }
                    for (std::list<char*>::iterator iter_hist = history_cmds.begin(); iter_hist != history_cmds.end(); ++iter_hist)
                    {
                      free(*iter_hist);
                    }
                    exit(0);
                }
                else
                {
                    illegal_cmd=TRUE;
                }
            }
        }
	}
	else if(!strcmp(cmd,"diff"))
    {
        if(num_arg!=2)
        {
            perror("smash error");
            illegal_cmd=TRUE;
        }
        else
        {


            int f1 = open(args[1],O_RDONLY);
            int f2 = open(args[2],O_RDONLY);
            if(f1<0 || f2<0)
            {
                perror("smash error");
                return -1;
            }
            char buf_file_1 [100];
            char buf_file_2 [100];
            int count_file_1=1;
            int count_file_2=1;
            bool check_identical=true;
            while(count_file_1>0 && count_file_2>0)
            {
                count_file_1 = read(f1,&buf_file_1,sizeof(buf_file_1));
                count_file_2 = read(f2,&buf_file_2,sizeof(buf_file_2));
                if((count_file_1 != count_file_2) ||(strcmp((const char*)&buf_file_1,(const char*)&buf_file_2)))
                    {
                        check_identical=false;
                        break;
                    }
            }
            close(f1);
            close(f2);
            if(check_identical)
                cout<<"0"<<endl;
            else
                cout << "1" <<endl;
        }
    }
	else // external command
	{
        cout << "entering external terminal" << endl;
 		ExeExternal(args, cmd);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
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
    cout<<cmdString<<endl;
    //cout << args[MAX_ARG] <<endl;
    cout << pwd << endl;
    
	int pID;
    switch(pID = fork())
	{
    		case -1:

					/* Could not create a child process , fork failed */
					perror("smash error : fork has failed");
					return;

        	case 0 :
                	/* Created a child process */
               		setpgrp(); /*By this we are changing the group of the new process to be itself leader , meaning if it is exited only
					   			this child process is stopped */

			        // Add your code here (execute an external command)
					execvp(cmdString,args);
					/*If reached here we got a problem excecuting the external command */
					perror("smash error : external cmd error");
					exit(0); // exits only the child process


			default:
                	/*If reached the default, we are the parent process , we should wait for the son process to finish */
					fg_pid = pID;
                    fg_name = (char *)malloc((strlen(args[0]) + 1) * sizeof(char));
                    strcpy(fg_name,args[0]);
					waitpid(pID,NULL,WUNTRACED); /*Waiting for the process to child process to finish */
					fg_pid = -1;
					free(fg_name);
	}
	return;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, jobs_table* jobs)
{
	char* Command;
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		/*Start after this */
		int i = 0 , num_arg = 0;
		Command = strtok(lineSize,delimiters);
		/* Empty Command */
		if (Command == NULL || (!strcmp(Command,"")))
        {
			perror("smash error: > Empty command");
			return -1;
		}
		args[0] = Command;
		for (i = 1; i < MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
			if (args[i] != NULL)
				num_arg++;
		}
		int pID;
		switch(pID = fork())
		{
		case -1 :
			perror("smash error : fork has failed");
			return -1;
		case 0 :
			setpgrp();
			execvp(args[0],args);
			perror("smash error : bg cmd error");
			exit(0); // exits only the child process

		default:	 /*Parent's process*/
				jobs->insert_new_job(Command,pID,false,false);
		}
		return 0;
	}
	return -1;
}

