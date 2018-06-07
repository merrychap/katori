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
#include "sniffing_utils.h"


#define BUF_SIZE         65536
#define INTERFACES_COUNT 100


static sniffer_t * sniffer_create();
static int sniffer_destroy(sniffer_t *sniffer);


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


int remove_interface(interface_t *interface) {
    interface->name = 0;
    free(interface);
    return 0;
}


int remove_interfaces(interface_t *interfaces, const size_t size) {
    for (size_t i = 0; i < size; i++)
        interfaces[i].name = 0;
    free(interfaces);
    return 0;
}


static int remove_settings(user_settings_t *settings) {
    if (settings) {
        remove_interface(settings->interface);
        free(settings);
    }
    return 0;
}


static void __process_packet(void *packet_struct) {
    packet_arg_t *packet = (packet_arg_t *) packet_struct;
    sniffer_t *sniffer   = packet->sniffer;

    pthread_mutex_lock(&(sniffer->lock));
    process_packet(sniffer, packet);
    pthread_mutex_unlock(&(sniffer->lock));
}


static void * __server_run(void * params) {
    server_t *server = (server_t *) params;

    struct sockaddr saddr;
    int saddr_size = 0;
    
    memset(&saddr, 0, sizeof(saddr));

    packet_arg_t packet;
    
    int data_size  = 0;
    int err_code   = 0;

    server->is_online = 1;

    while (server->is_online) {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(server->socket_fd, server->buffer, BUF_SIZE, 0, &saddr, (socklen_t*) &saddr_size);
        
        if (data_size < 0) { err_code = server_recv_error; break; }

        packet.buffer  = server->buffer;
        packet.size    = (size_t) data_size;
        packet.sniffer = server->sniffer;
        
        thread_pool_add(server->tpool, &__process_packet, &packet);
        // __process_packet(&packet);
    }
    
    pthread_exit(NULL);
    return NULL;
}


server_t * server_create(user_settings_t *settings) {
    if (settings == NULL ||
        settings->interface == NULL ||
        settings->logfile == NULL) return NULL;

    server_t  *server = (server_t *) malloc(sizeof(server_t));
    if (server == NULL) return NULL;

    server->socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    setsockopt(server->socket_fd , SOL_SOCKET , SO_BINDTODEVICE , settings->interface , strlen(settings->interface->name)+1);
    
    if (server->socket_fd < 0) { server_destroy(server); return NULL; }

    server->buffer  = (unsigned char *) malloc(BUF_SIZE);
    server->tpool   = thread_pool_init(MAX_THREADS, QUEUE_SIZE);
    server->sniffer = sniffer_create(settings->logfile);

    if (server->sniffer   == NULL ||
        server->tpool     == NULL ||
        server->buffer    == NULL) return NULL;
    
    server->is_online = 0;

    return server;
}


int server_run(server_t *server) {
    if (pthread_create(&(server->exec), NULL, __server_run, (void *)server) != 0) {
        return server_run_error;
    }
    return 0;
}


int server_destroy(server_t *server) {
    int err_code = 0;
    
    if (server == NULL) return server_null_error;
    
    server->is_online = 0;
    
    if (pthread_join(server->exec, NULL) != 0) err_code = server_exec_join_error;
    if (server->socket_fd >= 0) {
        // if (shutdown(server->socket_fd, SHUT_RDWR) < 0) perror("shutdown");
        if (close(server->socket_fd) < 0) perror("close");
    }
    
    if (server->buffer != NULL) free(server->buffer);
    
    thread_pool_kill(server->tpool, complete_shutdown);
    sniffer_destroy(server->sniffer);
    
    free(server);
    
    return err_code;
}


static sniffer_t * sniffer_create(FILE *logfile) {
    sniffer_t *sniffer = (sniffer_t *) malloc(sizeof(sniffer_t));
    
    if (pthread_mutex_init(&(sniffer->lock), NULL) != 0) return NULL;

    sniffer->logfile = logfile;
    sniffer->icmp    = 0;
    sniffer->tcp     = 0;
    sniffer->udp     = 0;
    sniffer->others  = 0;
    
    return sniffer;
}


static int sniffer_destroy(sniffer_t *sniffer) {
    if (sniffer == NULL) return -1;

    pthread_mutex_destroy(&(sniffer->lock));
    
    fclose(sniffer->logfile);
    free(sniffer);
    
    return 0;
}