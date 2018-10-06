#ifndef __NETLISTENER_H__
#define __NETLISTENER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <pthread.h>

#include <event2/event.h>

#include "packet.h"

struct katori_t;

typedef enum {
    LISTENER_NULL_PTR            = -1,
    SETTINGS_NULL_PTR            = -2,
    INTERFACE_NULL_PTR           = -3,
    LISTENER_RECV_ERROR          = -4,
    HANDLER_NULL_PTR             = -5,
    REALLOC_HANDLERS_FAILED      = -6,
    HANDLER_ARG_NULL_PTR         = -7,
    REALLOC_HANDLERS_ARGS_FAILED = -8,
    WRITE_ERROR                  = -9
} server_error_t;

struct netlistener_t {
    int com_fd;
    
    int fd;
    bool is_online;
    uint8_t *buffer;

    pthread_mutex_t lock;
    pthread_t *__listener;

    int mode;
    // FILE *logfile;
    const char *interface;
    
    struct event_base *evb;
    struct event *ev_pipe;
    struct event *ev_recv;
    struct event *ev_com;

    struct handler_t **__handlers;
    size_t __handlers_count;
    size_t __handlers_capacity;

    struct katori_t *katori;
};

struct handler_t {
    void (*handler)(struct packet_t *, void *);
    void *arg;

    bool active;
};

////////// HANDLER ////////////
struct handler_t * handler_new(void (*func)(struct packet_t *, void *),
    void *arg);

int handler_run(struct handler_t *handler, struct packet_t *pkt);
int handler_register(struct handler_t *handler);
int handler_unregister(struct handler_t *handler);

int handler_free(struct handler_t *handler);
///////////////////////////////

////////// LISTENER ////////////
struct netlistener_t * listener_new(const char *interface);

int listener_add_handler(struct netlistener_t *listener,
    struct handler_t *handler);
int listener_remove_handler(struct netlistener_t *listener,
    struct handler_t *handler);
struct handler_t *listener_get_handler(struct netlistener_t *listener,
    size_t index);

bool listener_online(struct netlistener_t *listener);
int listener_set_online(struct netlistener_t *listener);
int listener_set_offline(struct netlistener_t *listener);

int listener_run(struct netlistener_t *listener);
int listener_stop(struct netlistener_t *listener);
int listener_resume(struct netlistener_t *listener);

int listener_free(struct netlistener_t *listener);
////////////////////////////////
#endif