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
#include "utils.h"

#define BUF_SIZE         65536
#define INTERFACES_COUNT 100

#define INTERFACE_NAME_LEN 256

// static void
// __process_packet(void *packet_struct)
// {
//     packet_arg_t *packet = (packet_arg_t *) packet_struct;
//     sniffer_t *sniffer   = packet->sniffer;

//     process_packet(sniffer, packet);
// }

static void
listener_recv_cb(evutil_socket_t fd, short events, void *arg)
{
    struct netlistener_t *listener = arg;

    if (!listener->is_online)
        return;

    struct sockaddr saddr;
    int saddr_size = 0;
    int data_size  = 0;
    
    memset(&saddr, 0, sizeof(saddr));

    // struct packet_arg_t packet;

    listener->is_online = 1;

    saddr_size = sizeof(saddr);
    data_size = recvfrom(listener->fd, listener->buffer, BUF_SIZE, 0,
        &saddr, (socklen_t*) &saddr_size);
    
    if (data_size < 0)
        return;

    // packet.buffer  = server->buffer;
    // packet.size    = (size_t) data_size;
    // packet.sniffer = server->sniffer;
    
    // __process_packet(&packet);

}

struct netlistener_t *
listener_new(const char *interface)
{
    if (interface == NULL)
        return NULL;

    struct netlistener_t *listener = xmalloc_0(sizeof(struct netlistener_t));
    if (listener == NULL)
        return NULL;

    listener->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (setsockopt(listener->fd , SOL_SOCKET , SO_BINDTODEVICE,
        interface, strlen(interface) + 1) < 0)
        fatal("%s: failed to setsockopt. Try to use ROOT privs", __func__);
    
    listener->buffer = xmalloc_0(BUF_SIZE);

    if (listener->fd < 0)
        goto err2;
    
    if (listener->buffer == NULL)
        goto err1;

    long flags = fcntl(listener->fd, F_GETFL, 0);
    if (fcntl(listener->fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        goto err1;
    }

    if ((listener->evb = event_base_new()) == NULL)
        fatal("failed to create server event base");
    
    if ((listener->ev_recv = event_new(listener->evb, listener->fd, 
        EV_READ | EV_PERSIST, listener_recv_cb, listener)) == NULL)
        fatal("failed to create servant's event");
    
    listener->is_online = 0;

    return listener;

err1:
    close(listener->fd);
err2:
    free(listener);
    return NULL;
}

int
listener_run(const struct netlistener_t *listener)
{
    return 0;
}

int
listener_stop(struct netlistener_t *listener)
{
    return 0;
}

int
listener_resume(struct netlistener_t *listener)
{
    return 0;
}

int
listener_free(struct netlistener_t *listener)
{
    if (listener == NULL)
        return LISTENER_NULL_PTR;
    
    listener->is_online = 0;
    
    close(listener->fd);
    
    if (listener->buffer != NULL)
        free(listener->buffer);
    
    event_base_free(listener->evb);
    event_free(listener->ev_recv);
    
    free(listener);
    
    return 0;
}