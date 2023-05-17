/*
 * This is a very simple but fully featured SIP user agent, with the 
 * following capabilities:
 *  - SIP registration
 *  - Making and receiving call
 *  - Audio/media to sound device.
 *
 * Usage:
 *  - To make outgoing call, start simple_pjsua with the URL of remote
 *    destination to contact.
 *    E.g.:
 *       simpleua sip:user@remote
 *
 *  - Incoming calls will automatically be answered with 200.
 *
 * This program will quit once it has completed a single call.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pjsua-lib/pjsua.h>
#include "register_phone.h"

#define THIS_FILE       "APP"

/********************************************************************/
/*   function to register an endpoint with a SIP server             */
/*   Accepts username, password, and SIP URI                        */
/*   Returns a success or failure                                   */
/********************************************************************/

/********************************************************************/
/* Callback called by the library upon receiving incoming call      */
/********************************************************************/
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!", (int)ci.remote_info.slen, ci.remote_info.ptr));

    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
}
/********************************************************************/
/* Callback called by the library when call's state has changed     */
/********************************************************************/
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
                         (int)ci.state_text.slen,
                         ci.state_text.ptr));
}
/********************************************************************/
/* Callback called by the library when call's media state has changed */
/********************************************************************/
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
    }
}
/********************************************************************/
/* Display error and exit application */
/********************************************************************/
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}

/*
 * main()
 *
 * sip_uri[1] may contain URL to call.
 */
int register_endpoint(char* username, char* password, char* sip_uri, int dialed_number, char* sip_realm) //dialed_number is number to call?, sip_uri is URI
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    /* If argument is specified, it's got to be a valid SIP URL */
    if (dialed_number > 1) {
        // Create a pj_str_t object from the SIP URI
        status = pjsua_verify_url(sip_uri);
        if (status != PJ_SUCCESS) error_exit("Invalid URL in sip_uri", status);
    }

    /* Init pjsua */
    {
        pjsua_config cfg;
        pjsua_logging_config log_cfg;

        pjsua_config_default(&cfg);
        cfg.cb.on_incoming_call = &on_incoming_call;
        cfg.cb.on_call_media_state = &on_call_media_state;
        cfg.cb.on_call_state = &on_call_state;

        pjsua_logging_config_default(&log_cfg);
        log_cfg.console_level = 4;

        status = pjsua_init(&cfg, &log_cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;

        pjsua_transport_config_default(&cfg);
        cfg.port = 5060;
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg;

        pjsua_acc_config_default(&cfg);

        char cfg_id_build[100];
        strcpy(cfg_id_build, "sip:");
        strcat(cfg_id_build, username);
        strcat(cfg_id_build, "@");
        strcat(cfg_id_build, sip_uri);
        cfg.id = pj_str(cfg_id_build);
        cfg.reg_uri = pj_str(cfg_id_build);


        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str(sip_realm);
        cfg.cred_info[0].scheme = pj_str("basic");
        cfg.cred_info[0].username = pj_str(username);
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(password);

        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }

    printf("Press ENTER key to Continue\n");  
    getchar();  

    /* Destroy pjsua */
    pjsua_destroy();

    return 0;
}