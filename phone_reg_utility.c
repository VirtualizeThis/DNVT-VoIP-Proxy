/**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "register_phone.h"
#include "phone_reg_utility.h"
#include <pjsua-lib/pjsua.h>

int* main()
{
    char sip_uri[100];
    char sip_user[100];
    char sip_password[100];
    char sip_realm[100];
    int dialed_number;
    /*
    strcpy(sip_realm, "asterisk");
    strcpy(sip_user, "101");
    strcpy(sip_password, "password");
    */
    dialed_number = 0;
    
    printf("Enter SIP URI: ");
    fgets(sip_uri, sizeof(sip_uri), stdin);
    sip_uri[strcspn(sip_uri, "\n")] = '\0'; 
    
    printf("Enter SIP user: ");
    fgets(sip_user, sizeof(sip_user), stdin);
    sip_user[strcspn(sip_user, "\n")] = '\0';  
    
    printf("Enter SIP password: ");
    fgets(sip_password, sizeof(sip_password), stdin);
    sip_password[strcspn(sip_password, "\n")] = '\0';

    printf("Enter SIP Authentication Realm (Default: asterisk): ");
    fgets(sip_realm, sizeof(sip_realm), stdin);
    sip_realm[strcspn(sip_realm, "\n")] = '\0';  

    register_endpoint(sip_user, sip_password, sip_uri, dialed_number, sip_realm);
    
    return 0;
}