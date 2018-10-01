#ifndef _SNIFFING_H_
#define _SNIFFING_H_

#include <stdio.h>

#include "katori.h"
#include "cli_utils.h"
#include "netlistener.h"

typedef enum {
    SNIFFER_NULL_PTR = -1
} sniffing_mode_error_t;

struct sniffer_t {
    struct netlistener_t *listener;
    
    FILE * logfile;

    size_t icmp;
    size_t tcp;
    size_t udp;
    size_t others;
};

struct sniffer_t * sniffer_new(struct netlistener_t *listener);
int sniffer_free(struct sniffer_t *sniffer);

#endif