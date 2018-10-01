#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "katori.h"
#include "utils.h"
#include "cli.h"
#include "argparser.h"
#include "log.h"

#define PROJ_NAME "katori"

struct katori_config_t *
katori_config_new(void)
{
    struct katori_config_t *config = xmalloc_0(sizeof(struct katori_config_t));

    if (config == NULL)
        return NULL;

    return config;
}

int
katori_config_free(struct katori_config_t *config)
{
    if (config == NULL)
        return KATORI_CONFIG_NULL_PTR;

    free(config);

    return 0;
}

struct katori_t *
katori_new(struct katori_config_t *config)
{
    struct katori_t *katori = xmalloc_0(sizeof(struct katori_t));
    if (katori == NULL)
        return NULL;

    katori->config = config;
    
    if ((katori->listener = listener_new(config->interface)) == NULL) {
        free(katori);
        fatal("[-] failed to alloc network listener");
    }

    if ((katori->sniffer = sniffer_new(katori->listener)) == NULL) {
        free(katori->listener);
        free(katori);
        fatal("[-] failed to alloc sniffer");
    }

    return katori;
}

int
katori_run(const struct katori_t *katori)
{
    if (katori == NULL)
        return KATORI_NULL_PTR;

    start_prompt(katori);

    return 0;
}

int
katori_free(struct katori_t *katori)
{
    if (katori == NULL)
        return KATORI_NULL_PTR;

    katori->config = NULL; /* config will be freed in another function */

    free(katori);
    return 0;
}

int
main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: ./katori -i INTERFACE -l LOGFILE\n");
        exit(1);
    }
    
    log_init(PROJ_NAME, true);
    
    struct katori_config_t *config = katori_config_new();

    if (config == NULL)
        fatal("%s: failed to alloc katori config", __func__);

    switch (parse_args(argc, argv, config)) {
        case LOGFILE_NULL_PTR:
            fatal("[-] unspecified logfile");
            break;
        case LOGFILE_FOPEN_ERROR:
            fatal("[-] failed to open logfile");
            break;
        case INTERFACE_NULL_POINTER:
            fatal("[-] unspecified interface");
        default:
            break;
    }

    struct katori_t *katori = katori_new(config);

    if (katori == NULL)
        fatal("%s: failed to alloc katori", __func__);

    /* fire up the app */
    if (katori_run(katori) < 0)
        fatal("%s: failed to run katori", __func__);

    if (katori_free(katori) < 0)
        fatal("%s: failed to free katori", __func__);

    if (katori_config_free(config) < 0)
        fatal("%s: failed to free katori config", __func__);
    
    return 0;
}