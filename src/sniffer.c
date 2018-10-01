#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sniffer.h"
#include "utils.h"
#include "packet.h"
#include "log.h"

#define MAX_LOGFILE_LEN 1000

void
__process_packet(void *arg)
{
    struct packet_t *packet   = arg;
    struct sniffer_t *sniffer = packet->sniffer;

    if (process_packet(sniffer, packet) < 0)
        fatal("[-] failed to process a packet");
}

struct sniffer_t *
sniffer_new(struct netlistener_t *listener)
{
    if (listener == NULL)
        return NULL;
    
    struct sniffer_t *sniffer = xmalloc_0(sizeof(struct sniffer_t));

    if (sniffer == NULL)
        return NULL;

    sniffer->listener = listener;
    sniffer->logfile  = listener->logfile;
    
    sniffer->icmp     = 0;
    sniffer->tcp      = 0;
    sniffer->udp      = 0;
    sniffer->others   = 0;

    if (listener_add_handler(listener, __process_packet) < 0)
        fatal("[-] failed to add handler to network listener");
    
    return sniffer;
}

int
sniffer_free(struct sniffer_t *sniffer)
{
    if (sniffer == NULL)
        return SNIFFER_NULL_PTR;
    
    sniffer->logfile  = NULL;
    sniffer->listener = NULL;

    free(sniffer);
    
    return 0;
}