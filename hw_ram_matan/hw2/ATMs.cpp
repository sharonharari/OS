#include "ATMs.h"
/*Constructor  for each ATM */
ATM::ATM() {}
ATM::ATM(int input_atm_id, string input_atm_file ,bank* input_bank): ATM_id(input_atm_id+1),ATM_file_name(input_atm_file),our_Bank(input_bank){}


void ATM::run_atm_commands()
{
    string line;
    int args[4]={0};
    char atm_command;
    ifstream is(ATM_file_name.c_str());
    while(getline(is,line))
    {
        stringstream command_line(line);
        command_line >> atm_command >> args[0] >> args[1] >> args[2] >> args[3];
        switch(atm_command)
        {
            case 'O': our_Bank->bank_add_new_account(args[0],args[1],args[2],ATM_id);
                break;
            case 'D': our_Bank->bank_deposit_amount(args[0],args[1],args[2],ATM_id);
                break;
            case 'W': our_Bank->bank_withdraw_amount(args[0],args[1],args[2],ATM_id);
                break;
            case 'B': our_Bank->bank_check_account_balance(args[0],args[1],ATM_id);
                break;
            case 'T': our_Bank->bank_transfer_amount(args[0],args[1],args[2],args[3],ATM_id);
                break;
            case 'Q': our_Bank->bank_delete_account(args[0],args[1],ATM_id);
            default: break;
        }
        usleep(100000);
    }
}

