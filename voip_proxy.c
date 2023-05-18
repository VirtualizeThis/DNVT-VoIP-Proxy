/**/
#include "voip_proxy.h"

#include "sip_profile.h"

struct Sip_Profile_Args Sip_User_Profile;

/***************************************************************/
void clear_screen() {
    printf("\033[2J\033[H");
}
/***************************************************************/

/***************************************************************/
/*                      VoIP Proxy Main Menu         */
/***************************************************************/
void voip_main_ui() {
        clear_screen();
        printf("Menu:\n");
        printf("1. Register Phone to Profile\n");
        printf("2. Unregister Phone from Profile\n");
        printf("3. Quit\n");
        printf("Please enter your choice: ");
        fflush(stdout);
}
/***************************************************************/
/*                      Create VoIP Profile Function           */
/***************************************************************/

void create_profile_sip() {
    clear_screen();
    printf("\nEnter SIP URI: ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_uri);
    /*
    fgets(sip_uri, sizeof(sip_uri), stdin);
    sip_uri[strcspn(sip_uri, "\n")] = '\0'; 
*/
    printf("\nEnter SIP user: ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_user);

    /*
    fgets(sip_user, sizeof(sip_user), stdin);
    sip_user[strcspn(sip_user, "\n")] = '\0';  
*/
    printf("\nEnter SIP password: ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_password);
    /*
    fgets(sip_password, sizeof(sip_password), stdin);
    sip_password[strcspn(sip_password, "\n")] = '\0';
*/
    printf("\nEnter SIP Authentication Realm (Default: asterisk): ");
    fflush(stdout);
    scanf("%99s", Sip_User_Profile.sip_realm);
    /*
    fgets(sip_realm, sizeof(sip_realm), stdin);
    sip_realm[strcspn(sip_realm, "\n")] = '\0';
    */ 
   Sip_User_Profile.dialed_number = 0;
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
}
/********************************************************************/
/* VoIP Proxy Process -  Callback called by the library when call's state has changed     */
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

    printf("Press ENTER key to Continue\n");  
    getchar();  

    /* Destroy pjsua */
    pjsua_destroy();
    return 0;
}
/********************************************************************************/
/*                          Core Process                                      */
/********************************************************************************/
void* voip_main()
{
    bool quit = false;
    int MenuChoice;
    while (!quit) {
    voip_main_ui();
/********************************************************************************/
/*                      Menu Select - Basic or VoIP                             */
/********************************************************************************/
    scanf("%d", &MenuChoice);

    switch (MenuChoice) {
        case 1:
            printf("Register Phone to Profile selected.\n");
            create_profile_sip();
            // Create an instance of the ThreadArgs structure and set the values
            pthread_t th2;
            pthread_create(&th2, NULL, (void* (*)(void*))VoIP_Bridge, (void*) &Sip_User_Profile);
            break;
        case 2:
            printf("Unregister Phone from Profile selected.Curently does nothing.\n");
            // Call the function for unregistering the phone from the profile
            break;
        case 3:
            printf("Quitting\n");
            quit = true;
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
    while ((getchar()) != '\n');
    }
/********************************************************************************/
return 0;
}