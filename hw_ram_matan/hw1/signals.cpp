// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: signals_handlers func */
// taking care of SIGTSTP and SIGINT
#include "signals.h"
using namespace std;

void signals_handlers (int sigNum)
{

 if (fg_pid != -1)
 {

    if(sigNum == SIGTSTP )
    {
       if(kill(fg_pid,SIGTSTP) == 0) 
       {
           for (list<job>::iterator iter = jobs->jobs_list.begin(); iter != jobs->jobs_list.end(); ++iter) 
            {
               iter->update_job_status();
					if(iter->job_pid==fg_pid)
               {
                  exists_in_jobs_table = TRUE;
               }
            }
          if(!exists_in_jobs_table)
          {
    		   jobs->insert_new_job(fg_name, fg_pid, true, false);
          }
       }
       else
       {
          perror("smash error> SIGTSTP didnt work");
          return;
       }
      fg_pid = -1;
      exists_in_jobs_table = FALSE;
    }
   if(sigNum == SIGINT )
    {
      if(kill(fg_pid,SIGINT) == 0)
      { 
         fg_pid = -1;
         exists_in_jobs_table = FALSE;
      }
      else
      {
         perror("smash error > SIGINT didnt work");
      }
   }
 }
 else
{ 
   return;
}
return;
}
