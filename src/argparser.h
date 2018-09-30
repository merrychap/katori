#ifndef __ARGPARSER_H__
#define __ARGPARSER_H__

#include "katori.h"

struct katori_config_t;

int parse_args(int argc, char **argv, struct katori_config_t *config);

#endif