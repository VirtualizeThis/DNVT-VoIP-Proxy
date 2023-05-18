/**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "register_phone.h"
#include <pjsua-lib/pjsua.h>
#include <pthread.h>
#include <stdbool.h>


#ifndef VOIP_PROXY_H
#define VOIP_PROXY_H

void* voip_main();

#endif