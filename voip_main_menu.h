/**/
#ifndef VOIP_MAIN_MENU_H
#define VOIP_MAIN_MENU_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "register_phone.h"
#include <pjsua-lib/pjsua.h>
#include <pthread.h>
#include <stdbool.h>

void* voip_main_menu();

#endif