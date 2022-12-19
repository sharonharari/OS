
#include "jobs.h"

//Constructor for a new job
job::job(int job_index_r, char *job_name_r, int job_pid_r, bool job_stopped_r, bool job_finished_r) : job_index(job_index_r), job_pid(job_pid_r), job_stopped(job_stopped_r), job_finished(job_finished_r)
{
    /*
    if (job_name_r == NULL)
    {
        return -1;
    }
    */
    char *temp = (char *)malloc((strlen(job_name_r) + 1) * sizeof(char));
/*    if (temp == NULL)
    {
        return -1;
    }
    */
    strcpy(temp, job_name_r);
    job_name = temp;
    //Return current time
    timer = time(NULL);
}

/*** Updating the status of a specific job ***/
void job::update_job_status()
{
    int job_status;
    int ret = waitpid(job_pid, &job_status, WNOHANG | WUNTRACED | WCONTINUED);

        // * Checking return values * //
        if (ret == -1)
    {
        this->job_finished = true; // the desired job is finished
    }

    if (ret > 0)
    {
        //*Checking if the whole job is normally or was finished by interrupt raising *//
        if (WIFEXITED(job_status) || WIFSIGNALED(job_status))
        {
            this->job_finished = true;
        }

        //*Checking if the job was stopped * //
        if(WIFSTOPPED(job_status))
        {
            this->job_stopped = true;
            //this->job_finished = false;
        }
        // * the job was certainly not stopped * //
        else {
            this->job_stopped = false;
        }
    }
}

void job::print_current_job()
{
    /*First we make sure job is updated befor printing it out */
    this->update_job_status();
    /*Check if job is not finished yet, if its also stopped add additional word (Stopped) */
    if (!(this->job_finished) && !(this->job_stopped))
    {
        cout << "[" << job_index << "] " << job_name << " : " << job_pid << " " << difftime(time(NULL), timer) << " secs";
    }
    if (!(this->job_finished) && (this->job_stopped))
    {
        cout << "[" << job_index << "] " << job_name << " : " << job_pid << " " << difftime(time(NULL), timer) << " secs (Stopped)";
    }
}
//Constructor for a new jobs table
jobs_table::jobs_table()
{
    num_of_jobs = 0;
    //list <job> jobs_list_head;
}

void jobs_table::insert_new_job(char *job_name_r, int job_pid_r, bool job_stopped_r, bool job_finished_r)
{

    if (job_name_r == NULL)
    {
        return ;
    }
    this->num_of_jobs += 1;
    /*Inserting new job to the end of the jobs list */
    jobs_list.push_back(job(num_of_jobs, job_name_r, job_pid_r, job_stopped_r, job_finished_r));
}

void jobs_table::print_jobs()
{
    list<job>::iterator iter;
    /*iterating over the jobs list */
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        iter->print_current_job();
        cout<<endl;
    }
}
int jobs_table::get_job_PID(char *string_numb)
{
    list<job>::iterator iter;
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        /*Making sure job is updated befor using the job's info */
        iter->update_job_status();
        /*Trying to find the pid matching the numb recieved , else we throw an error*/
        try
        {
            if (iter->job_index == atoi(string_numb) && (!(iter->job_finished)))
            {
                return iter->job_pid;
            }
        }
        catch (...)
        { /*Catch all kind of possible throws */
            cout << "smash error: atoi has failed" << endl;
            return -1;
        }
    }
    //cout << "smash error: could not find the pid" << endl;
    return -1;
}

char *jobs_table::get_PID_name(char *string_numb)
{
    list<job>::iterator iter;
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        /*Making sure job is updated befor using the job's info */
        iter->update_job_status();
        try
        {
            if (iter->job_index == atoi(string_numb) && (!(iter->job_finished)))
            {
                return iter->job_name;
            }
        }
        catch (...)
        { /*Catch all kind of possible throws */
            cout << "smash error: atoi has failed" << endl;
            return NULL;
        }
    }
    //cout << "smash error: could not find the pid" << endl;
    return NULL;
}

int jobs_table::get_recent_PID()
{
    int temp_pid = -1;
    list<job>::iterator iter;
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        /*Making sure job is updated befor using the job's info */
        iter->update_job_status();
        /*get the latest job that is not finished*/
        if (!(iter->job_finished))
        {
            
            temp_pid = iter->job_pid;
        }
    }
    if (temp_pid != -1)
    {
        return temp_pid;
    }
    //cout << "smash error: could not find the pid" << endl;
    return -1;
    // }
    // jobs_list.reverse();
    // for(iter = jobs_list.begin();iter != jobs_list.end();++iter){
    //     /*Making sure job is updated befor using the job's info */
    //     iter->update_job_status();
    //     /*get the latest job that is not finished*/
    //     if(!(iter->job_finished)){
    //         /*return the list to it's original order*/
    //         jobs_list.reverse();
    //         return iter->job_pid;
    //     }

    // }
    // cout << "smash error: could not find the pid" << endl;
    // return -1;
}

char *jobs_table::get_recent_job_name()
{
    list<job>::iterator iter;
    jobs_list.reverse();
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        /*Making sure job is updated befor using the job's info */
        iter->update_job_status();
        /*get the latest job that is not finished*/
        if (!(iter->job_finished))
        {
            /*return the list to it's original order*/
            jobs_list.reverse();
            return iter->job_name;
        }
    }
    jobs_list.reverse();
    //cout << "smash error: could not find the pid" << endl;
    return NULL;
    // }
    // jobs_list.reverse();
    // for(iter = jobs_list.begin();iter != jobs_list.end();++iter){
    //     /*Making sure job is updated befor using the job's info */
    //     iter->update_job_status();
    //     /*get the latest job that is not finished*/
    //     if(!(iter->job_finished)){
    //         /*return the list to it's original order*/
    //         jobs_list.reverse();
    //         return iter->job_pid;
    //     }

    // }
    // cout << "smash error: could not find the pid" << endl;
    // return -1;
}

int jobs_table::get_recent_stopped_PID()
{
    list<job>::iterator iter;
    int temp_pid = -1;
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        iter->update_job_status();
        if (!(iter->job_finished) && (iter->job_stopped))
        {
            temp_pid = iter->job_pid;
        }
    }
    if (temp_pid != -1)
    {
        return temp_pid;
    }
    //cout << "smash error: could not find the pid" << endl;
    return -1;
}

char *jobs_table::get_recent_stopped_name()
{
    list<job>::iterator iter;
    jobs_list.reverse();
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        /*Making sure job is updated befor using the job's info */
        iter->update_job_status();
        /*get the latest job that is not finished*/
        if ((!(iter->job_finished)) && (iter->job_stopped))
        {
            /*return the list to it's original order*/
            jobs_list.reverse();
            return iter->job_name;
        }
    }
    jobs_list.reverse();
   // cout << "smash error: could not find the pid" << endl;
    return NULL;
}

bool jobs_table::get_stopped_status(char *string_numb)
{
    list<job>::iterator iter;
    for (iter = jobs_list.begin(); iter != jobs_list.end(); ++iter)
    {
        /*Making sure job is updated befor using the job's info */
        iter->update_job_status();
        try{
            if((iter->job_index == atoi(string_numb) && (iter->job_stopped)))
            {
                return true;
            }
        }
        catch(...){
            cout << "smash error: atoi has failed" << endl;
            return false;
        }
    }
    return false;
}
