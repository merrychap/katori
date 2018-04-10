#ifndef _CLI_H_
#define _CLI_H_


#include <stdio.h>
#include "cli_utils.h"


typedef enum {
    sniffer_start_error = -1,
    setup_error         = -2,
    interface_set_error = -3
} prompt_error_t;


int start_prompt();


#endif