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
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>

#include "netlistener.h"
#include "log.h"
#include "utils.h"
#include "packet.h"

#define BUF_SIZE         65536
#define INTERFACES_COUNT 100

#define INTERFACE_NAME_LEN 256

#define ONLINE_COMMAND  "ON"
#define OFFLINE_COMMAND "OFF"

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

    struct packet_t packet;

    listener->is_online = 1;

    saddr_size = sizeof(saddr);
    data_size = recvfrom(listener->fd, listener->buffer, BUF_SIZE, 0,
        &saddr, (socklen_t*) &saddr_size);
    
    if (data_size < 0)
        return;

    packet.buffer  = listener->buffer;
    packet.size    = (size_t) data_size;
    
    for (size_t i = 0; i < listener->__handlers_count; i++) {
        listener->__handlers[i](&packet, listener->__handlers_args[i]);
    }

}

static void
listener_com_cb(evutil_socket_t fd, short events, void *arg)
{
    struct netlistener_t *listener = arg;

    ssize_t bytes;
    uint8_t buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    bytes = read(listener->com_fd, buf, sizeof(buf));

    if (bytes < 0)
        return;

    /* if we need to exit event base loop */
    if (memcmp(buf, OFFLINE_COMMAND, bytes) == 0) {
        event_base_loopexit(listener->evb, NULL);
    }
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

    if ((listener->com_fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
        goto err1;

    long flags = fcntl(listener->fd, F_GETFL, 0);
    if (fcntl(listener->fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        goto err1;
    }

    if ((listener->evb = event_base_new()) == NULL)
        fatal("failed to create listener event base");
    
    if ((listener->ev_recv = event_new(listener->evb, listener->fd, 
        EV_READ | EV_PERSIST, listener_recv_cb, listener)) == NULL)
        fatal("failed to create recv event");

    if ((listener->ev_com = event_new(listener->evb, listener->com_fd,
        EV_READ | EV_PERSIST, listener_com_cb, listener)) == NULL)
        fatal("failed to create communication event");

    if (event_add(listener->ev_com, NULL) < 0)
        fatal("failed to add communication event");

    if (pthread_mutex_init(&listener->lock, NULL) != 0)
        fatal("failed to init lock");
    
    listener_set_offline(listener);

    return listener;

err1:
    close(listener->fd);
err2:
    free(listener);
    return NULL;
}

int
listener_add_handler(struct netlistener_t *listener,
    void (*handler)(struct packet_t *, void *), void *arg)
{
    if (listener == NULL)
        return LISTENER_NULL_PTR;
    if (handler == NULL)
        return HANDLER_NULL_PTR;

    size_t handlers_count    = listener->__handlers_count;
    size_t handlers_capacity = listener->__handlers_capacity;
    
    size_t handlers_arg_count    = listener->__handlers_args_count;
    size_t handlers_arg_capacity = listener->__handlers_args_capacity;

    assert(handlers_count == handlers_arg_count);
    assert(handlers_capacity == handlers_arg_capacity);

    void *buf_handlers = NULL;
    void *buf_args     = NULL;
    
    void (**handlers)(struct packet_t *, void *) = listener->__handlers;
    void **handlers_args = listener->__handlers_args;
    
    if (handlers_count >= handlers_capacity) {
        if (handlers_capacity == 0) {
            listener->__handlers = xmalloc_0(sizeof(handlers));
            listener->__handlers_args = xmalloc_0(sizeof(handlers_args));
            
            if (listener->__handlers == NULL)
                return HANDLER_NULL_PTR;

            if (listener->__handlers_args == NULL)
                return HANDLER_ARG_NULL_PTR;
            
            listener->__handlers_capacity = 1;
            listener->__handlers_args_capacity = 1;
        } else {
            buf_handlers = realloc(handlers, 2 * handlers_capacity * sizeof(handlers));
            if (buf_handlers == NULL) {
                free(handlers);
                return REALLOC_HANDLERS_FAILED;
            }

            buf_args = realloc(handlers_args, 2 * handlers_arg_capacity * sizeof(handlers_args));
            if (buf_args == NULL) {
                free(handlers_args);
                return REALLOC_HANDLERS_ARGS_FAILED;
            }

            listener->__handlers = buf_handlers;
            listener->__handlers_args = buf_args;
            
            listener->__handlers_capacity *= 2;
            listener->__handlers_args_capacity *= 2;
        }
    }
    
    listener->__handlers[listener->__handlers_count++] = handler;
    listener->__handlers_args[listener->__handlers_args_count++] = arg;

    return 0;
}

int
listener_remove_handler(struct netlistener_t *listener,
    void (*handler)(struct packet_t *, void *))
{
    if (listener == NULL)
        return LISTENER_NULL_PTR;
    if (handler == NULL)
        return HANDLER_NULL_PTR;

    for (size_t i = 0; i < listener->__handlers_count; i++) {
        if (handler == listener->__handlers[i]) {
            /* TODO: complete removing */
        }
    }

    return 0;
}

bool
listener_online(struct netlistener_t *listener)
{
    bool online = false;

    if (pthread_mutex_lock(&listener->lock) != 0)
        fatal("failed to lock listener mutex");

    online = listener->is_online;

    if (pthread_mutex_unlock(&listener->lock) != 0)
        fatal("failde to unlock listener mutex");
    
    return online;
}

int
listener_set_online(struct netlistener_t *listener)
{
    if (pthread_mutex_lock(&listener->lock) != 0)
        fatal("failed to lock listener mutex");

    listener->is_online = true;

    if (pthread_mutex_unlock(&listener->lock) != 0)
        fatal("failde to unlock listener mutex");

    return 0;
}

int
listener_set_offline(struct netlistener_t *listener)
{
    if (pthread_mutex_lock(&listener->lock) != 0)
        fatal("failed to lock listener mutex");

    listener->is_online = false;

    if (pthread_mutex_unlock(&listener->lock) != 0)
        fatal("failde to unlock listener mutex");

    return 0;
}

static void *
__listener_run_inner(void *arg)
{
    struct netlistener_t *listener = arg;

    listener_set_online(listener);

    if (event_add(listener->ev_recv, NULL) < 0)
        fatal("[-] failed to add netlistener recv event");
    
    if (event_base_dispatch(listener->evb) < 0)
        fatal("failed to dispatch network listener");

    pthread_exit(NULL);
    return NULL;
}

int
listener_run(struct netlistener_t *listener)
{
    pthread_t listen_run;

    if (pthread_create(&listen_run, NULL,
        __listener_run_inner, (void *) listener) != 0)
        fatal("%s: failed to create listener_run thread");

    if (pthread_join(listen_run, NULL) != 0)
        fatal("%s: failed to join listener_run");

    return 0;
}

int
listener_stop(struct netlistener_t *listener)
{
    if (!listener_online(listener))
        return 0;

    ssize_t bytes;

    bytes = write(listener->com_fd, OFFLINE_COMMAND, strlen(OFFLINE_COMMAND));

    if (bytes < 0)
        return WRITE_ERROR;

    listener_set_offline(listener);

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
    
    listener->is_online = false;
    
    close(listener->fd);
    
    free(listener->buffer);
    free(listener->__handlers);
    free(listener->__handlers_args);
    
    event_base_free(listener->evb);
    
    event_free(listener->ev_recv);
    event_free(listener->ev_com);

    if (pthread_mutex_destroy(&listener->lock) != 0)
        fatal("failed to destroy listener lock");

    listener->logfile = NULL;
    listener->evb     = NULL;
    listener->ev_recv = NULL;
    
    listener->__handlers     = NULL;
    listener->__handlers_args = NULL;

    listener->katori = NULL;
    
    free(listener);
    
    return 0;
}