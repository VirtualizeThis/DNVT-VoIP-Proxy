/**/
#ifndef REGISTER_ENDPOINT_H
#define REGISTER_ENDPOINT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pjsua-lib/pjsua.h>

struct Sip_Profile_Args {
char sip_user[256];
char sip_password[256];
char sip_uri[256];
int dialed_number;
char sip_realm[256];
};

extern struct Sip_Profile_Args Sip_User_Profile;

void register_endpoint(struct Sip_Profile_Args Sip_Profile);

#endif // REGISTER_ENDPOINT_H