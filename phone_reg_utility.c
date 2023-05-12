#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "register_phone.h"

char* main()
{
    char sip_uri[100];
    char sip_user[100];
    char sip_password[100];
    
    printf("Enter SIP URI: ");
    fgets(sip_uri, sizeof(sip_uri), stdin);
    sip_uri[strcspn(sip_uri, "\n")] = '\0';  // remove newline character
    
    printf("Enter SIP user: ");
    fgets(sip_user, sizeof(sip_user), stdin);
    sip_user[strcspn(sip_user, "\n")] = '\0';  // remove newline character
    
    printf("Enter SIP password: ");
    fgets(sip_password, sizeof(sip_password), stdin);
    sip_password[strcspn(sip_password, "\n")] = '\0';  // remove newline character
    
    char* result = register_endpoint(sip_uri, sip_user, sip_password);
    printf("%s\n", result);
    
    char* dynamic_result = malloc(strlen(result) + 1);  // allocate memory for dynamic string
    strcpy(dynamic_result, result);  // copy result to dynamic string
    
    return dynamic_result;
}