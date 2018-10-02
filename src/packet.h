#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>

struct netlistener_t;

typedef enum {
    PACKET_NULL_PTR = -1
} packet_error_t;

struct packet_t {
    uint8_t *buffer;
    size_t size;
};

void write_raw_data(FILE *logfile, unsigned char* data, size_t size);
void write_ethernet_header(FILE * logfile, struct packet_t *packet);
void write_ip_header(FILE * logfile, struct packet_t *packet);
void write_tcp_packet(FILE *logfile, struct packet_t *packet);
void write_udp_packet (FILE *logfile, struct packet_t *packet);
void write_arp_packet(FILE *logfile, struct packet_t *packet);
void write_icmp_packet(FILE *logfile, struct packet_t *packet);

#endif