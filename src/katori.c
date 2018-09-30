#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "katori.h"
#include "utils.h"
#include "cli.h"
#include "argparser.h"
#include "log.h"

struct katori_config_t *
katori_config_new(void)
{
    struct katori_config_t *config = xmalloc_0(sizeof(struct katori_config_t));

    if (config == NULL)
        return NULL;

    return config;
}

int
katori_config_del(struct katori_config_t *config)
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
    
    if ((katori->listener = listener_new()) == NULL)
        fatal("%s: failed to alloc network listener", __func__);

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
katori_del(struct katori_t *katori)
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
    struct katori_config_t *config = katori_config_new();
    struct katori_t *katori = katori_new(config);

    if (config == NULL)
        fatal("%s: failed to alloc katori config", __func__);

    if (katori == NULL)
        fatal("%s: failed to alloc katori", __func__);

    switch (parse_args(argc, argv, config)) {
        case LOGFILE_NULL_PTR:
            fatal("[-] unspecified logfile");
            break;
        case LOGFILE_FOPEN_ERROR:
            fatal("[-] failed to open logfile");
            break;
        default:
            break;
    }

    /* fire up the app */
    if (katori_run(katori) < 0)
        fatal("%s: failed to run katori", __func__);

    if (katori_del(katori) < 0)
        fatal("%s: failed to free katori", __func__);

    if (katori_config_del(config) < 0)
        fatal("%s: failed to free katori config", __func__);
    
    return 0;
}