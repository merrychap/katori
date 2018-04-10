#ifndef _SNIFFING_H_
#define _SNIFFING_H_


#include <stdio.h>

#include "cli_utils.h"
#include "server.h"


typedef struct user_settings_t user_settings_t;
typedef struct interface_t     interface_t;


void print_sniffing_header();
void print_sniffing_menu();

int sniffing_mode();


#endif