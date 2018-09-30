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

#include "netlistener.h"
#include "log.h"

#define BUF_SIZE         65536
#define INTERFACES_COUNT 100

#define INTERFACE_NAME_LEN 256

// don't forget to free this pointer!
static struct interface_t *
get_all_interfaces(size_t *size)
{
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

static struct interface_t *
interface_new(void)
{
    struct interface_t *interface = xmalloc_0(sizeof(struct interface_t));
    interface->name = xmalloc_0()
}

static int
interface_del(struct interface_t *interface)
{
    if (interface == NULL)
        return INTERFACE_NULL_PTR;
    
    free(interface->name);
    free(interface);
    
    return 0;
}

static void
__process_packet(void *packet_struct)
{
    packet_arg_t *packet = (packet_arg_t *) packet_struct;
    sniffer_t *sniffer   = packet->sniffer;

    process_packet(sniffer, packet);
}

static void
server_recv_cb(evutil_socket_t fd, short events, void *arg)
{
    struct server_t *server = arg;

    struct sockaddr saddr;
    int saddr_size = 0;
    
    memset(&saddr, 0, sizeof(saddr));

    struct packet_arg_t packet;
    
    int data_size  = 0;
    int err_code   = 0;

    server->is_online = 1;

    while (server->is_online) {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(server->fd, server->buffer, BUF_SIZE, 0, &saddr, (socklen_t*) &saddr_size);
        
        if (data_size < 0) {
            err_code = SERVER_RECV_ERROR;
            break;
        }

        packet.buffer  = server->buffer;
        packet.size    = (size_t) data_size;
        packet.sniffer = server->sniffer;
        
        // __process_packet(&packet);
    }
    
    return err_code;
}

struct server_t *
server_new(const struct interface_t *interface)
{
    if (interface == NULL ||
        settings->interface == NULL ||
        settings->logfile == NULL) {
        
        return NULL;
    }

    struct server_t *server = xmalloc_0(sizeof(struct server_t));
    if (server == NULL)
        return NULL;

    server->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    setsockopt(server->fd , SOL_SOCKET , SO_BINDTODEVICE , settings->interface->name, strlen(settings->interface->name)+1);
    
    server->buffer = xmalloc_0(BUF_SIZE);

    if (server->fd < 0)
        goto err2;
    
    if (server->buffer == NULL)
        goto err1;

    long flags = fcntl(server->fd, F_GETFL, 0);
    if (fcntl(server->fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        goto err1;
    }

    if ((server->evb = event_base_new()) == NULL)
        fatal("failed to create server event base");
    
    if ((server->ev_recv = event_new(server->evb, server->fd, 
        EV_READ | EV_PERSIST, server_recv_cb, server)) == NULL)
        fatal("failed to create servant's event");
    
    server->is_online = 0;

    return server;

err1:
    close(server->fd);
err2:
    free(server);
    return NULL;
}

int
server_run(const struct server_t *server)
{
    
    return 0;
}

int
server_stop(struct server_t *server)
{
    return 0;
}

int
server_resume(struct server_t *server)
{
    return 0;
}

int
server_del(struct server_t *server)
{
    int err_code = 0;
    
    if (server == NULL)
        return SERVER_NULL_PTR;
    
    server->is_online = 0;
    
    close(server->fd);
    
    if (server->buffer != NULL)
        free(server->buffer);
    
    free(server);
    
    return err_code;
}