#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip_simple.h>
#include <pjsua-lib/pjsua.h>
#include <pjsip/sip_endpoint.h>
#include <pjsip/sip_transport.h>
#include <pjmedia-codec.h>
#include <pjnath.h>
#include <pjmedia/errno.h>
#include <pjmedia/sdp_neg.h>
#include <pjmedia/sdp.h>
#include <pjsip-simple/evsub.h>
#include <pjsip-simple/pidf.h>
#include <pjsip-simple/presence.h>
#include <pjsip-simple/publish.h>
#include <pjsip/sip_autoconf.h>
#include <pjsip/sip_config.h>
#include <pjsip/sip_types.h>
#include <errno.h>
/********************************************************************/
/*   function to register an endpoint with a SIP server             */
/*   Accepts username, password, and SIP URI                        */
/*   Returns a success or failure                                   */
/********************************************************************/
char* register_endpoint(const char* username, char* password, char* sip_uri) {

    pjsua_config cfg;
    pjsua_config_default(&cfg);
    pjsua_logging_config log_cfg;
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config media_cfg;
    pjsua_media_config_default(&media_cfg);

    pj_status_t status;
    pjsip_endpoint *endpt_1;
    pj_caching_pool cp;

/********************************************************************/
/*       Validate and sanitize username, not pj_username            */
/********************************************************************/
    if (username == NULL || strlen(username) == 0 || strcspn(username, ";:,") != strlen(username)) {
        pj_shutdown();
        return "Invalid username";
}
    
    // Validate and sanitize sip_uri
    if (sip_uri == NULL || strlen(sip_uri) == 0 || strcspn(sip_uri, ";:,") != strlen(sip_uri)) {
        pj_shutdown();
        return "Invalid SIP URI";
}
/********************************************************************/
/********************************************************************/
/*        First get username, password to correct pj_ptr_t struct   */
/********************************************************************/

    pj_str_t pj_username = pj_str((char*)username);
    pj_str_t pj_password = pj_str((char*)password);
    pj_str_t pj_basic = pj_str((char*)"basic");
    pj_str_t pj_realm = pj_str((char*)"domain");

/********************************************************************/
/********************************************************************/
/*        This is the correct order to start pj_init()              */
/********************************************************************/
    status = pj_init();
    if (status != PJ_SUCCESS) {
         return "Error initializing PJSIP library";
    }
/********************************************************************/
/*                  Logging Config                                  */
/********************************************************************/
    status = pj_log_init();
        if (status != PJ_SUCCESS) {
         return "Error"; // allocate and return a duplicate string
    }
    pj_log_set_level(5); // Set max log level
    
/********************************************************************/
/*                Initialize network subsystem                      */
/********************************************************************/
    status = pjlib_util_init();
        if (status != PJ_SUCCESS) {
         return "Error"; // allocate and return a duplicate string
    }
/********************************************************************/
/*           This is the correct order to start pjsua_init()        */
/********************************************************************/
    status = pjsua_create();
        if (status != PJ_SUCCESS) {
        return "Error"; // allocate and return a duplicate string
    }
/********************************************************************/
/*            pjsua_init()                                          */
/********************************************************************/
    cfg.max_calls = 100;
    cfg.thread_cnt = 1;
    cfg.cred_count = 1; // Set the number of credentials
    cfg.cred_info[0].realm = pj_realm;
    cfg.cred_info[0].scheme = pj_basic;
    cfg.cred_info[0].username = pj_username;
    cfg.cred_info[0].data_type = 1;
    cfg.cred_info[0].data = pj_password;
    cfg.nameserver_count = 1;
    cfg.nameserver[0] = pj_str("10.10.10.10");
    
    cfg.force_lr = PJ_TRUE; // Force the use of "Loose Routing" (LR)
    cfg.require_100rel = PJ_TRUE; // Require the use of 100rel (RFC 3262) for reliable provisional responses
    cfg.use_timer = PJ_TRUE; // Enable the Session Timer mechanism (RFC 4028)
    cfg.enable_unsolicited_mwi = PJ_TRUE; // Enable unsolicited Message Waiting Indication (MWI) notifications

    cfg.user_agent = pj_str("My SIP Client"); // Set the User-Agent header value
    cfg.use_srtp = PJ_TRUE; // Enable Secure Real-time Transport Protocol (SRTP) for media encryption
    cfg.srtp_secure_signaling = PJ_TRUE; // Use SRTP for signaling encryption
    cfg.srtp_optional_dup_offer = PJ_TRUE; // Allow duplicate offers in SDP negotiation when using SRTP
    cfg.hangup_forked_call = PJ_FALSE; // Do not hang up a forked (attended transfer) call when one of the forked dialogs ends
    cfg.enable_upnp = PJ_TRUE; // Enable UPnP (Universal Plug and Play) for NAT traversal
    cfg.upnp_if_name = pj_str("eth0"); // Set the network interface name to be used for UPnP

    log_cfg.console_level = 4; // Set the required fields for log_cfg // Log level 4 for console output
    log_cfg.level = 5; // Log level 5 for logging output
    log_cfg.log_filename = pj_str("fuckyeah.log");

    // Set the required fields for media_cfg
    media_cfg.clock_rate = 8000;
    media_cfg.snd_clock_rate = 16000;
    media_cfg.ec_options = 0;

    status = pjsua_init(&cfg, &log_cfg, &media_cfg);
    if (status != PJ_SUCCESS) {
        return "Error"; // allocate and return a duplicate string
        }

printf("Error: %s\n", strerror(errno)); // <-- Bad
/********************************************************************/
/*           pjnath_init()                                          */
/********************************************************************/
    status = pjnath_init();
    if (status != PJ_SUCCESS) {
        return "Error"; // allocate and return a duplicate string
        }
/********************************************************************/
/*              Create PJSIP Endpoint                               */
/********************************************************************/
    pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 10240);
    
    status = pjsip_endpt_create(&cp.factory, NULL, &endpt_1);
    if (status != PJ_SUCCESS) {
        pjsip_endpt_destroy(endpt_1);
        pj_shutdown();
        return "Error creating SIP endpoint";
}
/********************************************************************/
/*              Create SIP transport                                */
/********************************************************************/
    pjsua_transport_id transport;
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    transport_cfg.port = 5060;
    transport_cfg.bound_addr = pj_str("10.10.10.22"); // static set the ip the program uses on the client machine BAD IDEA
    status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, &transport); // This fixed the pjsua_acc_add bug!
    if (status != PJ_SUCCESS) {
        pjsip_endpt_destroy(endpt_1);
        pj_shutdown();
        return "Error creating SIP transport";
    }

    if (status != PJ_SUCCESS) {
        return "Error initializing PJSUA library";
    }
/********************************************************************/

/********************************************************************/
    // Create SIP account
    pjsua_acc_id acc_id_var;
    acc_id_var = 5;
    pjsua_acc_config acc_cfg;
    pjsua_acc_config_default(&acc_cfg);
    char sip_id[PJSIP_MAX_URL_SIZE];
    pj_ansi_sprintf(sip_id, "sip:%s@%s", username, sip_uri);
    acc_cfg.id = pj_str(sip_id);
    char reg_uri[PJSIP_MAX_URL_SIZE];
    pj_ansi_sprintf(reg_uri, "sip:%s", sip_uri);

    // Define a variable to store the account cfg
    acc_cfg.reg_uri = pj_str(reg_uri);
    acc_cfg.cred_count = 1;
    acc_cfg.cred_info[0].realm = pj_realm;
    acc_cfg.cred_info[0].scheme = pj_basic;
    acc_cfg.cred_info[0].username = pj_username;
    acc_cfg.cred_info[0].data_type = 1;
    acc_cfg.cred_info[0].data = pj_password;
    acc_cfg.register_on_acc_add = PJ_FALSE;

/************************************************************************/

    status = pjsua_acc_add(&acc_cfg, PJ_FALSE, &acc_id_var);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id_var);
        pj_shutdown();
        return "Error adding SIP account";
    }
/************************************************************************/

    pjsua_state state = pjsua_get_state();

    const char* stateStr;
    switch (state) {
        case PJSUA_STATE_NULL:
            stateStr = "NULL";
            break;
        case PJSUA_STATE_CREATED:
            stateStr = "CREATED";
            break;
        case PJSUA_STATE_INIT:
            stateStr = "INIT";
            break;
        case PJSUA_STATE_STARTING:
            stateStr = "STARTING";
            break;
        case PJSUA_STATE_RUNNING:
            stateStr = "RUNNING";
            break;
        default:
            stateStr = "UNKNOWN";
}

    printf("PJSUA state: %s\n", stateStr);
 
/************************************************************************/
/************************************************************************/
/*          Register with Server                                        */
/************************************************************************/
/*    status = pjsua_acc_set_online_status(acc_id_var, PJ_TRUE);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id_var);
        pj_shutdown();
        return "Error registering account";
    }
*/
    status = pjsua_acc_set_registration(acc_id_var, PJ_TRUE);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id_var);
        pj_shutdown();
        return "Error registering SIP endpoint";
    }

/************************************************************************/
/*          Start pjsua after                                           */
/************************************************************************/
    status = pjsua_start();
        if (status != PJ_SUCCESS) {
        return "Error"; 
    }
/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

    // Cleanup and shutdown PJSIP library
    pjsua_acc_del(acc_id_var);
    pj_shutdown();

    return "Endpoint registration successful";
}