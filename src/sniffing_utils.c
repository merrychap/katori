#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#include "sniffing_utils.h"


static void parse_tcp_packet () {  }
static void parse_udp_packet () {  }
static void parse_arp_packet () {  }
static void parse_icmp_packet() {  }


void process_packet(sniffer_t *sniffer, packet_arg_t *packet) {
    struct iphdr *ip_header = (struct iphdr *) (packet->buffer + sizeof(struct ethhdr));
    
    switch (ip_header->protocol) {
        case 1:
            sniffer->icmp++;
            break;
        case 6:
            sniffer->tcp++;
            break;
        case 17:
            sniffer->udp++;
            break;
        default:
            sniffer->others++;
            break;
    }
}