#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip_simple.h>
#include <pjsua-lib/pjsua.h>
#include <pjsip/sip_endpoint.h>


// Function to register an endpoint with a SIP server
// Accepts username, password, and SIP URI
// Returns a success or failure string
char* register_endpoint(char* username, char* password, char* sip_uri) {
    // Initialize PJSIP library
    pj_status_t status;
    status = pj_init();
    if (status != PJ_SUCCESS) {
         return strdup("Error initializing PJSIP library"); // allocate and return a duplicate string
    }

    // Create memory pool factory for endpoint_cfg
    pj_caching_pool cache_pool_one;
    pj_pool_factory pool_factory_one;
    pj_pool_t *pool_one;

    pj_caching_pool_init(&cache_pool_one, NULL, 1024);  // Create a caching pool

    pool_factory_one.create_pool(&pool_factory_one, "my_pool", 1000, 1000, NULL); // Create a pool factory using the caching pool

    pool_one = pj_pool_create(&pool_factory_one, "my_pool", 1000, 1000, NULL); // Create a pool using the factory

    // Use the pool for your memory allocation needs

    // Create PJSIP endpoint
    pjsip_endpoint *endpoint;
    status = pjsip_endpt_create(&pool_factory_one, NULL, &endpoint);
    if (status != PJ_SUCCESS) { 
        pj_shutdown();
        return strdup("Error creating PJSIP endpoint"); // allocate and return a duplicate string
}


    // Create SIP transport
    pjsip_transport* transport;
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    status = pjsip_udp_transport_start(endpoint, NULL, NULL, 1, &transport);
    if (status != PJ_SUCCESS) {
        pjsip_endpt_destroy(endpoint);
        pj_shutdown();
        return strdup("Error creating SIP transport");
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
    acc_cfg.reg_uri = pj_str(reg_uri);
    acc_cfg.cred_count = 1;
    acc_cfg.cred_info[0].realm = pj_str("*");
    acc_cfg.cred_info[0].scheme = pj_str("digest");
    acc_cfg.cred_info[0].username = pj_str(username);
    acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    acc_cfg.cred_info[0].data = pj_str(password);
    status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id);
        pjsip_transport_shutdown(transport);
        pjsip_endpt_destroy(endpoint);
        pj_shutdown();
        return strdup("Error adding SIP account");
    }

    // Register endpoint with SIP server
    status = pjsua_acc_set_registration(acc_id, PJ_TRUE);
    if (status != PJ_SUCCESS) {
        pjsua_acc_del(acc_id);
        pjsip_transport_shutdown(transport);
        pjsip_endpt_destroy(endpoint);
        pj_shutdown();
        return strdup("Error registering SIP endpoint");
    }

    // Cleanup and shutdown PJSIP library
    pj_pool_release(pool_one);
    pjsua_acc_del(acc_id);
    pjsip_transport_shutdown(transport);
    pjsip_endpt_destroy(endpoint);
    pj_shutdown();

    return strdup("Endpoint registration successful");
}