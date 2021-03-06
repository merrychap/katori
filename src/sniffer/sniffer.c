#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#include "sniffer.h"
#include "utils.h"
#include "packet.h"
#include "log.h"

#define MAX_LOGFILE_LEN 1000

void
sniffer_capture_packet(struct packet_t *packet, void *arg)
{
    struct sniffer_t *sniffer = arg;

    if (sniffer == NULL)
        return;
    if (packet == NULL)
        return;
    
    FILE * logfile          = sniffer->logfile;
    struct iphdr *ip_header = (struct iphdr *) (packet->buffer + sizeof(struct ethhdr));

    switch (ip_header->protocol) {
        case 1:
            sniffer->icmp++;
            write_icmp_packet(logfile, packet);
            break;
        case 6:
            write_tcp_packet(logfile, packet);
            sniffer->tcp++;
            break;
        case 17:
            write_udp_packet(logfile, packet);
            sniffer->udp++;
            break;
        default:
            sniffer->others++;
            break;
    }
}

struct sniffer_t *
sniffer_new(struct netlistener_t *listener,
    FILE *logfile)
{
    if (listener == NULL)
        return NULL;
    
    struct sniffer_t *sniffer = xmalloc_0(sizeof(struct sniffer_t));

    if (sniffer == NULL)
        return NULL;

    sniffer->listener = listener;
    sniffer->logfile  = logfile;
    
    sniffer->icmp     = 0;
    sniffer->tcp      = 0;
    sniffer->udp      = 0;
    sniffer->others   = 0;

    if ((sniffer->__handler = handler_new(sniffer_capture_packet, sniffer)) < 0)
        fatal("[-] failed to allocate handler");

    if (listener_add_handler(listener, sniffer->__handler) < 0)
        fatal("[-] failed to add handler to network listener");

    fprintf(sniffer->logfile, "YOOOO\n");
    
    return sniffer;
}

int
sniffer_start(struct sniffer_t *sniffer)
{
    if (sniffer == NULL)
        return SNIFFER_NULL_PTR;

    if (handler_register(sniffer->__handler) < 0)
        fatal("[-] failed to register sniffer event");
    
    return 0;
}

int
sniffer_stop(struct sniffer_t *sniffer)
{
    if (sniffer == NULL)
        return SNIFFER_NULL_PTR;

    if (handler_unregister(sniffer->__handler) < 0)
        fatal("[-] failed to unregister sniffer event");
    
    return 0;
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