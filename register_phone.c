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


// function to register an endpoint with a SIP server
// Accepts username, password, and SIP URI
// Returns a success or failure string
char* register_endpoint(char* username, char* password, char* sip_uri) {
    pjsua_config cfg;
    pjsua_config_default(&cfg);
    pjsua_logging_config log_cfg;
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config media_cfg;
    pjsua_media_config_default(&media_cfg);
    pjsua_transport_config trans_cfg;
    pjsua_transport_config_default(&trans_cfg);

    pj_status_t status;
    pjsip_endpoint *endpt_1;
    pj_caching_pool cp;

    // Initialize PJSIP library
    status = pj_init();
    if (status != PJ_SUCCESS) {
         return strdup("Error initializing PJSIP library"); // allocate and return a duplicate string
    }
    status = pj_log_init();
        if (status != PJ_SUCCESS) {
         return strdup("Error"); // allocate and return a duplicate string
    }
    status = pjlib_util_init();
        if (status != PJ_SUCCESS) {
         return strdup("Error"); // allocate and return a duplicate string
    }
   
    pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 10240);
    
    // Create PJSIP endpoint
    status = pjsip_endpt_create(&cp.factory, NULL, &endpt_1);
    if (status != PJ_SUCCESS) {
        pjsip_endpt_destroy(endpt_1);
        pj_shutdown();
        return strdup("Error creating SIP endpoint");
}

    // Create SIP transport
    pjsip_transport* transport;
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    status = pjsip_udp_transport_start(endpt_1, NULL, NULL, 1, &transport);
    if (status != PJ_SUCCESS) {
        pjsip_endpt_destroy(endpt_1);
        pj_shutdown();
        return strdup("Error creating SIP transport");
    }

    pj_log_set_level(4); // Set max log level

   
    pj_log_set_level(3); // Reset log level
    if (status != PJ_SUCCESS) {
        return strdup("Error initializing PJSUA library");
    }


    // Validate and sanitize username
    if (username == NULL || strlen(username) == 0 || strcspn(username, ";:,") != strlen(username)) {
        pj_shutdown();
        return strdup("Invalid username");
}
    
    // Validate and sanitize sip_uri
    if (sip_uri == NULL || strlen(sip_uri) == 0 || strcspn(sip_uri, ";:,") != strlen(sip_uri)) {
        pj_shutdown();
        return strdup("Invalid SIP URI");
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
    acc_cfg.cred_info[0].username = pj_str(username);
    acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    acc_cfg.cred_info[0].data = pj_str(password);
    acc_cfg.register_on_acc_add = PJ_FALSE;

    status = pjsua_acc_add(&acc_cfg, 0, &acc_id);
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