#include <pjlib.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_simple.h>
#include <pjsua-lib/pjsua.h>
#include <pjmedia/sdp.h>
#include <pjmedia/sdp_neg.h>
#include <pjmedia/session.h>
#include <pjsua-lib/pjsua.h>
#include <pjmedia-codec.h>
#include <pjsua.h>
#include <pj/types.h>
#include <string.h>
#include <pj/log.h>

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
    pj_log_set_level(3);
    pj_caching_pool cp;
    pj_caching_pool_init(&cp, NULL, 4096);
    
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);
    
    // Answer the call automatically
    pjsua_call_answer(call_id, 200, NULL, NULL);
}

static void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
    pj_log_set_level(3);
    pj_caching_pool cp;
    pj_caching_pool_init(&cp, NULL, 4096);
    
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);
}

void make_call(const char *dest_uri) {
    pj_status_t status;
    pj_str_t uri = pj_str((char*)dest_uri);

    // Make outgoing call
    pjsua_call_id call_id;
    status = pjsua_call_make_call(pjsua_acc_get_default(), &uri, 0, NULL, NULL, &call_id);
    if (status != PJ_SUCCESS) {
    }
}

void init_sip() {
    pj_log_set_level(3);
    pj_caching_pool cp;
    pj_caching_pool_init(&cp, NULL, 4096);

    pj_status_t status;

    /* Initialize pjsua */
    status = pjsua_create();
    if (status != PJ_SUCCESS) {
        return;
    }

    // Configure pjsua
    pjsua_config cfg;
    pjsua_config_default(&cfg);
    cfg.cb.on_incoming_call = &on_incoming_call;
    cfg.cb.on_call_state = &on_call_state;

    // Initialize pjsua with configured options
    status = pjsua_init(&cfg, NULL, NULL);
    if (status != PJ_SUCCESS) {
        pjsua_destroy();
        return;
    }

    // Add UDP transport
    pjsua_transport_config tp_cfg;
    pjsua_transport_config_default(&tp_cfg);
    tp_cfg.port = 5060;
    status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &tp_cfg, NULL);
    if (status != PJ_SUCCESS) {
        pjsua_destroy();
        return;
    }

    // Start pjsua
    status = pjsua_start();
    if (status != PJ_SUCCESS) {
        pjsua_destroy();
        return;
    }
}

void make_sip_call(const char *dest_uri) {
    init_sip();
    make_call(dest_uri);
}

void cleanup_sip() {
    pjsua_destroy();
}