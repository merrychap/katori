#ifndef _CLI_H_
#define _CLI_H_

#include <stdio.h>

#include "katori.h"
#include "cli_utils.h"

struct katori_t;

typedef enum {
    NOP = -1
} prompt_error_t;

int start_prompt(struct katori_t *katori);


#endif