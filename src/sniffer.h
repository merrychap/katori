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

struct packet_arg_t {
    struct sniffer_t *sniffer;
    unsigned char *buffer;
    size_t size;
};

struct sniffer_t * sniffer_new(FILE *logfile);
int sniffer_free(struct sniffer_t *sniffer);

int sniffing_mode(const struct katori_t *katori);

#endif