/**/
#ifndef REGISTER_AND_DIAL_FUNC_H
#define REGISTER_AND_DIAL_FUNC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pjsua-lib/pjsua.h>
#include "sip_profile.h"

void register_and_dial_func(struct Sip_Profile_Args Sip_Profile);

#endif // REGISTER_AND_DIAL_FUNC_H