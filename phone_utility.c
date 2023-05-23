/***************************************************************/
/*                   Phone Test Utility            */
/***************************************************************/

#include "phone_utility.h"
struct Sip_Profile_Args Sip_User_Profile;

/***************************************************************/
/*           Register Phone, Dial Number Function              */
/*                  Dial Number Disabled                        */
/***************************************************************/

void Get_SIP_User_Profile() {

    clear();
    echo();
    mvprintw(0, 0, "Enter SIP URI: ");
    getstr(Sip_User_Profile.sip_uri);
    
    mvprintw(1, 0, "Enter SIP user: ");
    getstr(Sip_User_Profile.sip_user); 
    
    mvprintw(2, 0, "Enter SIP password: ");
    getstr(Sip_User_Profile.sip_password);
    
    mvprintw(3, 0, "Enter SIP Authentication Realm (Default: asterisk): ");
    getstr(Sip_User_Profile.sip_realm);

    mvprintw(4, 0, "Enter Number to Dial (Zero for Register Only): ");
    refresh();
    mvscanw(4, 47, "%d", &Sip_User_Profile.dialed_number);

    clear();
    refresh();
    noecho();
    
    register_and_dial_func(Sip_User_Profile);
}
/***************************************************************/

/***************************************************************/
/*                      Main Menu                              */
/***************************************************************/
void utility_main_ui() {
    clear();
    echo();
    bool quit = false;
    int MenuChoice;
    mvprintw(0, 0, "Menu:");
    mvprintw(1, 0, "1. Register and Dial Phone");
    mvprintw(2, 0, "2. Unused");
    mvprintw(3, 0, "3. Quit");
    mvprintw(4, 0, "Please enter your choice: ");
    refresh();

    scanw("%d", &MenuChoice);
    noecho();
    switch (MenuChoice) {
        case 1:
            clear();
            mvprintw(0, 0, "Register Phone to Profile and Dial.\n");
            Get_SIP_User_Profile();
            break;
        case 2:
            clear();
            mvprintw(0, 0, ("Quitting\n"));
            refresh();
            quit = true;
            break;
        case 3:
            clear();
            mvprintw(0, 0, "Quitting\n");
            quit = true;
            refresh();
            break;
        default:
            mvprintw(0, 0, "Invalid choice. Please try again.\n");
            clear();
            refresh();
            break;
    }
}
/***************************************************************/

int* main()
{
    bool quit = false;
    initscr();
    echo();
    while (!quit) {
        utility_main_ui();
        while ((getchar()) != '\n');
    }
    return 0;
}