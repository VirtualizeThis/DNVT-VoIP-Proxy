#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip_simple.h>
#include <pjsua-lib/pjsua.h>
#include <pjsip/sip_endpoint.h>
#include <pjsip/sip_transport.h>

// function to register an endpoint with a SIP server
// Accepts username, password, and SIP URI
// Returns a success or failure string
char* register_endpoint(char* username, char* password, char* sip_uri) {
    pjsua_config cfg;
    pjsua_logging_config log_cfg;
    pjsua_transport_config trans_cfg;
    pjsua_config_default(&cfg);
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config(media_cfg);
    pjsua_media_config_default(&media_cfg);
    pj_status_t status;

    // Initialize PJSIP library
    status = pj_init();
    if (status != PJ_SUCCESS) {
         return strdup("Error initializing PJSIP library"); // allocate and return a duplicate string
    }

    // Initialize PJSUA library

    status = pjsua_init(&cfg, &log_cfg, &media_cfg);
    pjsua_start();
    if (status != PJ_SUCCESS) {
        return strdup("Error initializing PJSUA library");
    }

    // Create SIP account
    pjsua_acc_id acc_id;
    pjsua_acc_config acc_cfg;
    pjsua_acc_config_default(&acc_cfg);
    char sip_id[PJSIP_MAX_URL_SIZE];
    pj_ansi_sprintf(sip_id, "sip:%s@%s", username, sip_uri);
    acc_cfg.id = pj_str(sip_id);
    char reg_uri[PJSIP_MAX_URL_SIZE];
    pj_ansi_sprintf(reg_uri, "sip:%s", sip_uri);
    // Define a variable to store the transport ID
    acc_cfg.reg_uri = pj_str(reg_uri);
    acc_cfg.cred_count = 1;
    acc_cfg.cred_info[0].realm = pj_str("*");
    acc_cfg.cred_info[0].scheme = pj_str("digest");
    acc_cfg.cred_info[0].username = pj_str(username);
    acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    acc_cfg.cred_info[0].data = pj_str(password);
    acc_cfg.register_on_acc_add = PJ_FALSE;
    status = pjsua_acc_add(&acc_cfg, PJ_FALSE, &acc_id);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id);
        pj_shutdown();
        return strdup("Error adding SIP account");
    }

    // Register endpoint with SIP server
    status = pjsua_acc_set_registration(acc_id, PJ_TRUE);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id);
        pj_shutdown();
        return strdup("Error registering SIP endpoint");
    }

    // Cleanup and shutdown PJSIP library
    pjsua_acc_del(acc_id);
    pj_shutdown();

    return strdup("Endpoint registration successful");
}