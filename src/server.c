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

#include "server.h"
#include "thread_pool.h"


#define BUF_SIZE         65536
#define INTERFACES_COUNT 100


struct server_t {
    int socket_fd;
    char *buffer;
    
    thread_pool_t *tpool;
};


typedef struct {
    size_t icmp;
    size_t tcp;
    size_t udp;
    size_t others;
} sniffer_t;


typedef struct {
    sniffer_t *sniffer;
    unsigned char *buffer;
    size_t size;
} packet_arg_t;


// don't forget to free this pointer!
char ** get_all_interfaces() {
    size_t index = 0;

    char **inf_names = (char *) malloc(INTERFACES_COUNT * sizeof(char *));
    
    ifaddrs_t *addrs    = 0;
    ifaddrs_t *tmp_addr = 0;

    getifaddrs(&addrs);
    tmp_addr = addrs;

    while (tmp_addr) {
        if (tmp_addr->ifa_addr && tmp_addr->ifa_addr->sa_family == AF_PACKET)
            inf_names[index++] = tmp_addr->ifa_name;
        tmp_addr = tmp_addr->ifa_next;
    }
    
    freeifaddrs(addrs);

    return inf_names;
}


int process_packet(void *packet_struct) {
    packet_arg_t *packet = (packet_arg_t *) packet_struct;
    sniffer_t *sniffer   = packet->sniffer;

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
    printf("TCP : %d   UDP : %d   ICMP : %d   Others : %d\r", sniffer->tcp, sniffer->udp, sniffer->icmp, sniffer->others);
}


server_t * server_create() {
    server_t  *server  = (server_t *)  malloc(sizeof(server_t));
    
    if (server == NULL) return NULL;

    server->socket_fd = socket(AF_INET, SOCK_RAW, htons(ETH_P_ALL));
    if (server->socket_fd < 0) { server_destroy(server); return NULL; }

    server->buffer = (char *) malloc(BUF_SIZE);

    server->tpool = thread_pool_init(MAX_THREADS, QUEUE_SIZE);

    return server;
}


int server_run(server_t *server) {
    struct sockaddr saddr;
    packet_arg_t packet;
    
    int saddr_size = sizeof(saddr);
    int data_size  = 0;
    int err_code   = 0;

    sniffer_t *sniffer = (sniffer_t *) malloc(sizeof(sniffer_t));

    while (1) {
        data_size = recvfrom(server->socket_fd, server->buffer, BUF_SIZE, 0, &saddr, (socklen_t *)&saddr_size);
        if (data_size < 0) { err_code = server_recv_error; break; }
        
        packet.buffer  = server->buffer;
        packet.size    = data_size;
        packet.sniffer = sniffer;
        
        thread_pool_add(server->tpool, &process_packet, &packet);
    }

    server_destroy(server);

    return err_code;
}


int server_destroy(server_t *server) {
    if (server->socket_fd) close(server->socket_fd);
    
    free(server->buffer);
    
    thread_pool_kill(server->tpool, complete_shutdown);
    
    free(server);
    return 0;
}