/**/
#include "voip_main_menu.h"
/***************************************************************/
void clear_screen() {
    printf("\033[2J\033[H");
}
/***************************************************************/


/***************************************************************/
/*                      VoIP Proxy Main Menu         */
/***************************************************************/
void voip_main_ui() {
        clear_screen();
        printf("Menu:\n");
        printf("1. Register Phone to Profile\n");
        printf("2. Unregister Phone from Profile\n");
        printf("3. Quit\n");
        printf("Please enter your choice: ");
        fflush(stdout);
}
/***************************************************************/
/*                      Create VoIP Profile Function           */
/***************************************************************/

void create_profile_sip() {
    clear_screen();
    printf("\nEnter SIP URI: ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_uri);
    /*
    fgets(sip_uri, sizeof(sip_uri), stdin);
    sip_uri[strcspn(sip_uri, "\n")] = '\0'; 
*/
    printf("\nEnter SIP user: ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_user);

    /*
    fgets(sip_user, sizeof(sip_user), stdin);
    sip_user[strcspn(sip_user, "\n")] = '\0';  
*/
    printf("Enter SIP password: ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_password);
    /*
    fgets(sip_password, sizeof(sip_password), stdin);
    sip_password[strcspn(sip_password, "\n")] = '\0';
*/
    printf("Enter SIP Authentication Realm (Default: asterisk): ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_realm);
    /*
    fgets(sip_realm, sizeof(sip_realm), stdin);
    sip_realm[strcspn(sip_realm, "\n")] = '\0';
    */ 
   Sip_User_Profile.dialed_number = 0;
}

/***************************************************************/
/*                      This is the core function              */
/***************************************************************/
void* voip_main_menu()
{
    bool quit = false;
    int MenuChoice;
    while (!quit) {
    voip_main_ui();
/********************************************************************************/
/*                      Menu Select - Basic or VoIP                             */
/********************************************************************************/
    scanf("%d", &MenuChoice);

    switch (MenuChoice) {
        case 1:
            printf("Register Phone to Profile selected.\n");
            create_profile_sip();

            // Create an instance of the ThreadArgs structure and set the values
            


            pthread_t th2;
            pthread_create(&th2, NULL, (void* (*)(void*))register_endpoint, (void*) &Sip_User_Profile);
            break;
        case 2:
            printf("Unregister Phone from Profile selected.Curently does nothing.\n");
            // Call the function for unregistering the phone from the profile
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
/********************************************************************************/
}