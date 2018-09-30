#ifndef __KATORI_H__
#define __KATORI_H__

#include <stdio.h>

#include "netlistener.h"

typedef enum {
    LOGFILE_NULL_PTR       = -1,
    LOGFILE_FOPEN_ERROR    = -2,
    KATORI_NULL_PTR        = -3,
    KATORI_CONFIG_NULL_PTR = -4
} katori_error_t;

struct katori_t {
    struct katori_config_t *config;

    struct netlistener_t *listener;
};

struct katori_config_t {
    const char *logfile;
    int verbose;
};

struct katori_config_t * katori_config_new(void);
int katori_config_del(struct katori_config_t *config);

struct katori_t * katori_new(struct katori_config_t *config);
int katori_run(const struct katori_t *katori);
int katori_del(struct katori_t *katori);

#endif