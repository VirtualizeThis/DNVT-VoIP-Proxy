/***************************************************************/
/*                   Phone Test Utility            */
/***************************************************************/

#include "phone_utility.h"
struct Sip_Profile_Args Sip_User_Profile;

/***************************************************************/

/***************************************************************/
/*                      Main Menu                              */
/***************************************************************/
void utility_main_ui() {
        clear_screen();
        printf("Menu:\n");
        printf("1. Register and Dial Phone\n");
        printf("2. Unused\n");
        printf("3. Quit\n");
        printf("Please enter your choice: ");
        fflush(stdout);
}
/***************************************************************/
/*           Register Phone, Dial Number Function              */
/*                  Dial Number Disabled                        */
/***************************************************************/

void Get_SIP_User_Profile() {
    Sip_User_Profile.dialed_number = 0; //Dial Number is hard disabled here with 0
    
    printf("Enter SIP URI: ");
    fgets(Sip_User_Profile.sip_uri, sizeof(Sip_User_Profile.sip_uri), stdin);
    Sip_User_Profile.sip_uri[strcspn(Sip_User_Profile.sip_uri, "\n")] = '\0'; 
    
    printf("Enter SIP user: ");
    fgets(Sip_User_Profile.sip_user, sizeof(Sip_User_Profile.sip_user), stdin);
    Sip_User_Profile.sip_user[strcspn(Sip_User_Profile.sip_user, "\n")] = '\0';  
    
    printf("Enter SIP password: ");
    fgets(Sip_User_Profile.sip_password, sizeof(Sip_User_Profile.sip_password), stdin);
    Sip_User_Profile.sip_password[strcspn(Sip_User_Profile.sip_password, "\n")] = '\0';

    printf("Enter SIP Authentication Realm (Default: asterisk): ");
    fgets(Sip_User_Profile.sip_realm, sizeof(Sip_User_Profile.sip_realm), stdin);
    Sip_User_Profile.sip_realm[strcspn(Sip_User_Profile.sip_realm, "\n")] = '\0';  

    register_and_dial_func(Sip_User_Profile);
}
/***************************************************************/

int* main()
{
    bool quit = false;
    int MenuChoice;
    while (!quit) {
    utility_main_ui();
    scanf("%d", &MenuChoice);

    switch (MenuChoice) {
        case 1:
            printf("Register Phone to Profile and Dial.\n");
            create_profile_sip();
            break;
        case 2:
            printf("Quitting\n");
            quit = true;
            break;
        case 3:
            printf("Quitting\n");
            quit = true;
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
    while ((getchar()) != '\n');
    }
    return 0;
}