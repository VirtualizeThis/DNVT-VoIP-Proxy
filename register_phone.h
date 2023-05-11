<<<<<<< HEAD
#ifndef REGISTER_ENDPOINT_H
#define REGISTER_ENDPOINT_H

char* register_endpoint(char* username, char* password, char* sip_uri);

#endif // REGISTER_ENDPOINT_H
=======
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjnath.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip_simple.h>
#include <pjsua-lib/pjsua.h>
#include <pjmedia/sdp.h>
#include <pjmedia/sdp_neg.h>
#include <pjmedia/session.h>
#include <pjmedia-codec.h>
#include <pj/types.h>
#include <string.h>
#include <pj/log.h>
#include <pjsip/sip_module.h>

// Function to register an endpoint with a SIP server
// Returns a success or failure string
// This function register an endpoint with a SIP server and returns a success or failure string
// It accepts username, password and URI
// It is called in main.c to register phones.

char* register_endpoint(const char* username, const char* password, const char* server_uri) {
    // Initialize PJSIP library
    pj_status_t status;
    pj_caching_pool cp;
    pj_pool_t *pool;
    pjsip_endpoint *endpoint;

    status = pj_init();
    if (status != PJ_SUCCESS) {
        return "Error initializing PJSIP library";
    }

    // Create PJSIP endpoint
    pj_caching_pool_init(&cp, NULL, 1024);
    pool = pj_pool_create(&cp.factory, "endpoint", 1024, 1024, NULL);

    if (pool == NULL) {
        pj_shutdown();
        return "Error creating pool";
    }

    // Create SIP transport
    pjsip_transport* transport;
    status = pjsip_udp_transport_start(endpoint, NULL, NULL, 5060, &transport);
    if (status != PJ_SUCCESS) {
        pjsip_endpt_destroy(endpoint);
        pj_shutdown();
        return "Error starting SIP transport";
    }

    // Create SIP account
    pjsua_acc_id acc_id;
    pjsua_acc_config cfg;
    pjsua_acc_config_default(&cfg);
    char buf[256];
    sprintf(buf, "sip:%s@%s", username, server_uri);
    cfg.id = pj_str(buf);
    char buf1[256];
    sprintf(buf1, "sip:%s", server_uri);
    cfg.reg_uri = pj_str(buf1);
    cfg.cred_count = 1;
    cfg.cred_info[0].realm = pj_str("*");
    cfg.cred_info[0].scheme = pj_str("digest");
    cfg.cred_info[0].username = pj_str((char*)username);
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].data = pj_str((char*)password);
    status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id);
        /* pjsip_udp_transport_start(transport, NULL); */
        pjsip_endpt_destroy(endpoint);
        pj_shutdown();
        return "Error adding SIP account";
    }

    // Register endpoint with SIP server
    status = pjsua_acc_set_registration(acc_id, PJ_TRUE);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id);
        /* pjsip_udp_transport_stop(transport); */
        pjsip_endpt_destroy(endpoint);
        pj_shutdown();
        return "Error registering SIP endpoint";
    }

    // Shutdown PJSIP library
    pjsip_endpt_destroy(endpoint);
    pj_shutdown();

    return "Endpoint registration successful";
}
>>>>>>> 8f64e825f2ab7b65a73a38fb7c7abe4f9e20433d
