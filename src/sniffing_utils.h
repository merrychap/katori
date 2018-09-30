#ifndef _SNIFFING_UTILS_H_
#define _SNIFFING_UTILS_H_

#include "sniffer.h"

void process_packet(struct sniffer_t *sniffer,
    struct packet_arg_t *packet);

#endif