#ifndef _CLI_H_
#define _CLI_H_


#include <stdio.h>

// #include "server.h"


typedef enum {
    sniffer_start_error = -1,
    setup_error         = -2,
    interface_set_error = -3
} prompt_error_t;

typedef struct user_settings_t user_settings_t;


user_settings_t * settings_setup();

int start_prompt(user_settings_t *settings);

char * set_interface();

char * get_cur_interface(user_settings_t *settings);


#endif