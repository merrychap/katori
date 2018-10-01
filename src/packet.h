#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>

#include "sniffer.h"

struct sniffer_t;
struct netlistener_t;

typedef enum {
    PACKET_NULL_PTR = -1
} packet_error_t;

struct packet_t {
    struct sniffer_t *sniffer;
    uint8_t *buffer;
    size_t size;
};

int process_packet(struct sniffer_t *sniffer,
    struct packet_t *packet);

#endif