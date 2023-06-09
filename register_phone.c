/* */
#include "register_phone.h"

#define THIS_FILE       "APP"

/********************************************************************/
/*   function to register an endpoint with a SIP server             */
/*   Accepts username, password, and SIP URI                        */
/*   Returns a success or failure                                   */
/********************************************************************/
/***************************************************************/

extern struct Sip_Profile_Args Sip_User_Profile;

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
 *char* username, char* password, char* sip_uri, int dialed_number, char* sip_realm
 * sip_uri[1] may contain URL to call.
 */ 
void register_and_dial_func(struct Sip_Profile_Args Sip_Profile) //dialed_number is number to call?, sip_uri is URI
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

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
        log_cfg.log_filename = pj_str("pj_log_phone_utility.log");


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
        strcat(cfg_id_build, Sip_Profile.sip_user);
        strcat(cfg_id_build, "@");
        strcat(cfg_id_build, Sip_Profile.sip_uri);
        cfg.id = pj_str(cfg_id_build);
        cfg.reg_uri = pj_str(cfg_id_build);


        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str(Sip_Profile.sip_realm);
        cfg.cred_info[0].scheme = pj_str("basic");
        cfg.cred_info[0].username = pj_str(Sip_Profile.sip_user);
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(Sip_User_Profile.sip_password);

        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) error_exit("Error adding account", status);

        /* If argument is specified, it's got to be a valid SIP URL */
        if (Sip_Profile.dialed_number > 0) {
        char dialed_number_str[32]; // Assuming the maximum length of the string representation
        // Convert the integer to a string
        sprintf(dialed_number_str, "%d", Sip_Profile.dialed_number);
        pj_str_t pj_dialed_number = pj_str(dialed_number_str);
        status = pjsua_call_make_call(acc_id, &pj_dialed_number, 0, NULL, NULL, NULL);
        if (status != PJ_SUCCESS) {
            error_exit("Error making call", status);
            sleep(100);
        }
        }
        
    }
    /* Destroy pjsua */
    pjsua_destroy();
}