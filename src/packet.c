#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#include "packet.h"

void
write_raw_data(FILE *logfile, unsigned char* data, size_t size)
{
    size_t chunk_size = 16;
    for (size_t chunk_id = 0; chunk_id + chunk_size < size; chunk_id += chunk_size) {
        fprintf(logfile, "    ");
        for (size_t byte_id = chunk_id; byte_id < chunk_id + chunk_size; byte_id++) {
            fprintf(logfile, " %02X", (unsigned int) data[byte_id]);
        }
        fprintf(logfile, "         ");
        for (size_t byte_id = chunk_id; byte_id < chunk_id + chunk_size; byte_id++) {
            if (data[byte_id] >= 32 && data[byte_id] <= 128) fprintf(logfile, "%c", data[byte_id]);
            else fprintf(logfile, ".");
        }
        fprintf(logfile, "\n");
    }
}

void
write_ethernet_header(FILE * logfile, struct packet_t *packet)
{
    struct ethhdr *eth = (struct ethhdr *)packet->buffer;
     
    fprintf(logfile, "\n");
    fprintf(logfile, "Ethernet Header\n");
    fprintf(logfile, "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0],   eth->h_dest[1],   eth->h_dest[2],   eth->h_dest[3],   eth->h_dest[4],   eth->h_dest[5]);
    fprintf(logfile, "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    fprintf(logfile, "   |-Protocol            : %u \n", (unsigned short) eth->h_proto);
}

void
write_ip_header(FILE * logfile, struct packet_t *packet)
{
    write_ethernet_header(logfile, packet);
   
    // unsigned short iphdrlen;
    struct sockaddr_in source, dest;
         
    struct iphdr *iph = (struct iphdr *)(packet->buffer + sizeof(struct ethhdr) );
    // iphdrlen = iph->ihl * 4;
     
    memset(&source, 0, sizeof(source));
    memset(&dest,   0, sizeof(dest));
    
    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr   = iph->daddr;
     
    fprintf(logfile, "\n");
    fprintf(logfile, "IP Header\n");
    fprintf(logfile, "   |-IP Version        : %d\n", (unsigned int) iph->version);
    fprintf(logfile, "   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int) iph->ihl, ((unsigned int)(iph->ihl))*4);
    fprintf(logfile, "   |-Type Of Service   : %d\n", (unsigned int) iph->tos);
    fprintf(logfile, "   |-IP Total Length   : %d  Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    fprintf(logfile, "   |-Identification    : %d\n", ntohs(iph->id));
    //fprintf(logfile , "   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //fprintf(logfile , "   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //fprintf(logfile , "   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    fprintf(logfile, "   |-TTL      : %d\n", (unsigned int) iph->ttl);
    fprintf(logfile, "   |-Protocol : %d\n", (unsigned int) iph->protocol);
    fprintf(logfile, "   |-Checksum : %d\n", ntohs(iph->check));
    fprintf(logfile, "   |-Source IP        : %s\n", inet_ntoa(source.sin_addr));
    fprintf(logfile, "   |-Destination IP   : %s\n", inet_ntoa(dest.sin_addr));
}

void
write_tcp_packet(FILE *logfile, struct packet_t *packet)
{
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)(packet->buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;
 
    struct tcphdr *tcph = (struct tcphdr*)(packet->buffer + iphdrlen + sizeof(struct ethhdr));
    int header_size     =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
     
    fprintf(logfile , "\n\n***********************TCP Packet*************************\n");
         
    write_ip_header(logfile, packet);
         
    fprintf(logfile, "\n");
    fprintf(logfile, "TCP Header\n");
    fprintf(logfile, "   |-Source Port      : %u\n",   ntohs(tcph->source));
    fprintf(logfile, "   |-Destination Port : %u\n",   ntohs(tcph->dest));
    fprintf(logfile, "   |-Sequence Number    : %u\n", ntohl(tcph->seq));
    fprintf(logfile, "   |-Acknowledge Number : %u\n", ntohl(tcph->ack_seq));
    fprintf(logfile, "   |-Header Length      : %d DWORDS or %d BYTES\n", (unsigned int) tcph->doff, (unsigned int) tcph->doff * 4);
    //fprintf(logfile , "   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
    //fprintf(logfile , "   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
    fprintf(logfile, "   |-Urgent Flag          : %d\n", (unsigned int) tcph->urg);
    fprintf(logfile, "   |-Acknowledgement Flag : %d\n", (unsigned int) tcph->ack);
    fprintf(logfile, "   |-Push Flag            : %d\n", (unsigned int) tcph->psh);
    fprintf(logfile, "   |-Reset Flag           : %d\n", (unsigned int) tcph->rst);
    fprintf(logfile, "   |-Synchronise Flag     : %d\n", (unsigned int) tcph->syn);
    fprintf(logfile, "   |-Finish Flag          : %d\n", (unsigned int) tcph->fin);
    fprintf(logfile, "   |-Window         : %d\n", ntohs(tcph->window));
    fprintf(logfile, "   |-Checksum       : %d\n", ntohs(tcph->check));
    fprintf(logfile, "   |-Urgent Pointer : %d\n", tcph->urg_ptr);
    fprintf(logfile, "\n");
    fprintf(logfile, "                        DATA Dump                         ");
    fprintf(logfile, "\n");
         
    fprintf(logfile, "IP Header\n");
    write_raw_data(logfile, packet->buffer, iphdrlen);
         
    fprintf(logfile, "TCP Header\n");
    write_raw_data(logfile, packet->buffer+iphdrlen, tcph->doff*4);
         
    fprintf(logfile , "Data Payload\n");
    size_t data_size = (packet->size > header_size) ? packet->size - header_size : 0;
    write_raw_data(logfile, packet->buffer + header_size, data_size);
                         
    fprintf(logfile , "\n###########################################################");
}

void
write_udp_packet (FILE *logfile, struct packet_t *packet)
{

}

void
write_arp_packet(FILE *logfile, struct packet_t *packet)
{

}

void
write_icmp_packet(FILE *logfile, struct packet_t *packet)
{

}