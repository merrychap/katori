#ifndef _CLI_H_
#define _CLI_H_

#include <stdio.h>
#include "cli_utils.h"

typedef enum {
    NOP = -1
} prompt_error_t;

int start_prompt();


#endif