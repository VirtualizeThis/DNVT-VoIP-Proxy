/**/
#include "voip_proxy.h"

#include "sip_profile.h"

struct Sip_Profile_Args Sip_User_Profile;

/***************************************************************/
/*                      VoIP Proxy Main Menu         */
/***************************************************************/
void voip_main_ui() {
        clear();
        echo();
        mvprintw(0, 0, "Menu:");
        mvprintw(1, 0, "1. Register Phone to Profile");
        mvprintw(2, 0, "2. Unregister Phone from Profile");
        mvprintw(3, 0, "3. Quit");
        mvprintw(4, 0, "Please enter your choice: ");
        refresh();
}
/***************************************************************/
/*                      Create VoIP Profile Function           */
/***************************************************************/

void create_profile_sip() {
    clear(); // Clear the screen

    mvprintw(0, 0, "Enter SIP URI: ");
    refresh();
    char sip_uri[100];
    scanw("%99s", sip_uri);

    mvprintw(1, 0, "Enter SIP user: ");
    refresh();
    char sip_user[100];
    scanw("%99s", sip_user);

    mvprintw(2, 0, "Enter SIP password: ");
    refresh();
    char sip_password[100];
    scanw("%99s", sip_password);

    mvprintw(3, 0, "Enter SIP Authentication Realm (Default: asterisk): ");
    refresh();
    char sip_realm[100];
    scanw("%99s", sip_realm);

    refresh();
}
/********************************************************************************/
/*                      VoIP Proxy Process Launch                               */
/********************************************************************************/
#define THIS_FILE       "APP"

/********************************************************************************/
/*   function to register an endpoint with a SIP server             */
/*   Accepts username, password, and SIP URI                        */
/*   Returns a success or failure                                   */
/********************************************************************************/
/********************************************************************************/



/********************************************************************************/
/* VoIP Proxy Process - Callback called by the library upon receiving incoming call      */
/********************************************************************************/
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
   pj_status_t status;
    status = pj_log_init();
    pj_log_set_level(5); // Set max log level
    
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!", (int)ci.remote_info.slen, ci.remote_info.ptr));

    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);

    // Connect call to system default audio speakers
    pjsua_conf_connect(ci.conf_slot, 0);
    pjsua_conf_connect(0, ci.conf_slot);
}
/********************************************************************/
/* VoIP Proxy Process -  Callback called by the library when call's media state has changed */
/********************************************************************/
static void on_call_media_state(pjsua_call_id call_id)
{
pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);

        // Create an audio channel back to the source of the call using the system microphone
        pjsua_conf_connect(ci.conf_slot, pjsua_call_get_conf_port(call_id));
    }
}
/********************************************************************/
/* VoIP Proxy Process -  on_call_stste */
/********************************************************************/
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
                         (int)ci.state_text.slen,
                         ci.state_text.ptr));

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        // Call has ended, run on_call_state() function
        on_call_state(call_id, NULL);
    }
}




/********************************************************************/
/* VoIP Proxy Process -  Display error and exit application */
/********************************************************************/
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}

/*
 * sip_uri[1] may contain URL to call.
 */ 
void* VoIP_Bridge(struct Sip_Profile_Args* Sip_Profile) //dialed_number is number to call?, sip_uri is URI
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    /* If argument is specified, it's got to be a valid SIP URL */
    if (Sip_Profile->dialed_number > 1) {
        // Create a pj_str_t object from the SIP URI
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

        char cfg_id_build[1000]; // Assuming an appropriate size for the array
        strcpy(cfg_id_build, "sip:");
        strcat(cfg_id_build, Sip_Profile->sip_user);
        strcat(cfg_id_build, "@");
        strcat(cfg_id_build, Sip_Profile->sip_uri);
        
        cfg.id = pj_str(cfg_id_build);
        cfg.reg_uri = pj_str(cfg_id_build);


        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str(Sip_Profile->sip_realm);
        cfg.cred_info[0].scheme = pj_str("basic");
        cfg.cred_info[0].username = pj_str(Sip_Profile->sip_user);
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(Sip_User_Profile.sip_password);

        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }
    return 0;
}
/********************************************************************************/
/*                          Core Process                                      */
/********************************************************************************/
void* voip_main()
{
    bool quit = false;
    int MenuChoice;
    initscr();
    raw();
    while (!quit) {
    voip_main_ui();

    scanw("%d", &MenuChoice);

    switch (MenuChoice) {
        case 1:
            mvprintw(5, 0, "Register Phone to Profile selected.");
            refresh();
            create_profile_sip();
            // Create an instance of the ThreadArgs structure and set the values
            pthread_t th2;
            pthread_create(&th2, NULL, (void* (*)(void*))VoIP_Bridge, (void*) &Sip_User_Profile);
            break;
        case 2:
            mvprintw(5, 0, "Unregister Phone from Profile selected. Currently does nothing.");
            refresh();
            // Call the function for unregistering the phone from the profile
            break;
        case 3:
            mvprintw(5, 0, "Quitting");
            refresh();
            quit = true;
            break;
        default:
            mvprintw(5, 0, "Invalid choice. Please try again.");
            refresh();
            break;
    }
    noecho();
    while ((getchar()) != '\n');
    }
/********************************************************************************/
    pjsua_destroy();
return 0;
}