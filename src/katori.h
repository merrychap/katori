#ifndef __KATORI_H__
#define __KATORI_H__

#include <stdio.h>

#include "netlistener.h"
#include "sniffer.h"

typedef enum {
    LOGFILE_NULL_PTR       = -1,
    LOGFILE_FOPEN_ERROR    = -2,
    KATORI_NULL_PTR        = -3,
    KATORI_CONFIG_NULL_PTR = -4,
    INTERFACE_NULL_POINTER = -5
} katori_error_t;

struct katori_t {
    struct katori_config_t *config;

    struct netlistener_t *listener;

    struct sniffer_t *sniffer;
};

struct katori_config_t {
    FILE *logfile;
    const char *interface;
    int verbose;
};

struct katori_config_t * katori_config_new(void);
int katori_config_free(struct katori_config_t *config);

struct katori_t * katori_new(struct katori_config_t *config);
int katori_run(const struct katori_t *katori);
int katori_free(struct katori_t *katori);

#endif