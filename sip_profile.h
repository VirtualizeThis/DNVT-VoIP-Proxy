#ifndef SIP_PROFILE_H
#define SIP_PROFILE_H

struct Sip_Profile_Args {
char sip_user[256];
char sip_password[256];
char sip_uri[256];
int dialed_number;
char sip_realm[256];
};

#endif //SIP_PROFILE_H