/**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pjsua-lib/pjsua.h>
#include <pthread.h>
#include <stdbool.h>
#include <ncurses.h>
#include <libusb.h>
#include <err.h>
#include <sys/queue.h>
#include <assert.h>

typedef struct {
    uint32_t data[20];
    uint8_t size;
} QUEUE;

typedef struct {
    char digits[20];
    uint8_t digit_count;
    uint8_t state;
    uint8_t connected_phone;
    QUEUE rxd;
    QUEUE txd;
    uint8_t pending_command;
    uint8_t line_state;
    uint16_t connected_device;
    uint32_t recording_index;
    bool playing_recording;
    int recording_number;
    struct timespec last_tx;
    pthread_mutex_t mutex;
} PHONE;

#ifndef VOIP_PROXY_H
#define VOIP_PROXY_H

void* voip_main(PHONE* phone);

#endif