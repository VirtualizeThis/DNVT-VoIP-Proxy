/***************************************************************/
/*                   Register Endpoint Test Utility            */
/***************************************************************/

#include "phone_reg_utility.h"

/***************************************************************/


int* main()
{
    struct Sip_Profile_Args Sip_User_Profile;
    Sip_User_Profile.dialed_number = 0;
    
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

    register_endpoint(Sip_User_Profile);
    
    return 0;
}