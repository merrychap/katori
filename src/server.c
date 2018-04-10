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
#include <pthread.h>

#include <fcntl.h>
#include <unistd.h>

#include "server.h"
#include "thread_pool.h"


#define BUF_SIZE         65536
#define INTERFACES_COUNT 100


struct server_t {
    int socket_fd;
    unsigned char *buffer;
    
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    
    thread_pool_t *tpool;
};


struct sniffer_t {
    pthread_mutex_t lock;
    
    size_t icmp;
    size_t tcp;
    size_t udp;
    size_t others;
};


typedef struct {
    sniffer_t *sniffer;
    unsigned char *buffer;
    size_t size;
} packet_arg_t;


sniffer_t * sniffer_create();
int sniffer_destroy(sniffer_t *sniffer);


// don't forget to free this pointer!
interface_t * get_all_interfaces(size_t *size) {
    size_t index = 0;

    interface_t *inf_names = (interface_t *) malloc(INTERFACES_COUNT * sizeof(interface_t));
    
    ifaddrs_t *addrs    = 0;
    ifaddrs_t *tmp_addr = 0;

    getifaddrs(&addrs);
    tmp_addr = addrs;

    while (tmp_addr) {
        if (tmp_addr->ifa_addr && tmp_addr->ifa_addr->sa_family == AF_PACKET)
            inf_names[index++].name = tmp_addr->ifa_name;
        tmp_addr = tmp_addr->ifa_next;
    }
    
    *size = index;

    freeifaddrs(addrs);
    return inf_names;
}


int remove_interfaces(interface_t *interfaces, const size_t size) {
    for (size_t i = 0; i < size; i++)
        interfaces[i].name = 0;
    free(interfaces);
    return 0;
}


void process_packet(void *packet_struct) {
    packet_arg_t *packet = (packet_arg_t *) packet_struct;
    sniffer_t *sniffer   = packet->sniffer;

    pthread_mutex_lock(&(sniffer->lock));

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
    printf("TCP : %lu   UDP : %lu   ICMP : %lu   Others : %lu\r", sniffer->tcp, sniffer->udp, sniffer->icmp, sniffer->others);
    pthread_mutex_unlock(&(sniffer->lock));
}


server_t * server_create(user_settings_t *settings) {
    server_t  *server  = (server_t *)  malloc(sizeof(server_t));
    
    if (server == NULL) return NULL;

    server->socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    
    if (server->socket_fd < 0) { server_destroy(server); return NULL; }

    server->buffer = (unsigned char *) malloc(BUF_SIZE);
    server->tpool  = thread_pool_init(MAX_THREADS, QUEUE_SIZE);

    return server;
}


int server_run(server_t *server) {
    struct sockaddr saddr;
    int saddr_size = 0;
    
    memset(&saddr, 0, sizeof(saddr));

    packet_arg_t packet;
    
    int data_size  = 0;
    int err_code   = 0;

    sniffer_t *sniffer = sniffer_create();
    if (sniffer == NULL) return sniffer_create_failure;

    // TODO add conditional variable for correct quiting
    while (1) {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(server->socket_fd, server->buffer, BUF_SIZE, 0, &saddr, (socklen_t*)&saddr_size);
        
        if (data_size < 0) { err_code = server_recv_error; break; }
        
        packet.buffer  = server->buffer;
        packet.size    = data_size;
        packet.sniffer = sniffer;
        
        thread_pool_add(server->tpool, &process_packet, &packet);
    }

    server_destroy (server);
    sniffer_destroy(sniffer);

    return err_code;
}


int server_destroy(server_t *server) {
    if (server == NULL) return -1;
    
    if (server->socket_fd >= 0) {
        if (shutdown(server->socket_fd, SHUT_RDWR) < 0) perror("shutdown");
        if (close(server->socket_fd) < 0) perror("close");
    }
    
    if (server->buffer != NULL) free(server->buffer);
    thread_pool_kill(server->tpool, complete_shutdown);
    free(server);
    return 0;
}


sniffer_t * sniffer_create() {
    sniffer_t *sniffer = (sniffer_t *) malloc(sizeof(sniffer_t));
    
    if (pthread_mutex_init(&(sniffer->lock), NULL) != 0) return NULL;

    sniffer->icmp   = 0;
    sniffer->tcp    = 0;
    sniffer->udp    = 0;
    sniffer->others = 0;
    
    return sniffer;
}


int sniffer_destroy(sniffer_t *sniffer) {
    if (sniffer == NULL) return -1;

    pthread_mutex_destroy(&(sniffer->lock));
    
    free(sniffer);
    return 0;
}