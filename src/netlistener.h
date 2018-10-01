#ifndef __NETLISTENER_H__
#define __NETLISTENER_H__

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include <event2/event.h>

struct katori_t;

typedef enum {
    LISTENER_NULL_PTR       = -1,
    SETTINGS_NULL_PTR       = -2,
    INTERFACE_NULL_PTR      = -3,
    LISTENER_RECV_ERROR     = -4,
    HANDLER_NULL_PTR        = -5,
    REALLOC_HANDLERS_FAILED = -6
} server_error_t;

struct netlistener_t {
    int fd;
    size_t is_online;
    uint8_t *buffer;

    int mode;
    FILE *logfile;
    const char *interface;
    
    struct event_base *evb;
    struct event *ev_pipe;
    struct event *ev_recv;

    void (**handlers)(void *);
    size_t handlers_count;
    size_t handlers_capacity;

    struct katori_t *katori;
};

struct netlistener_t * listener_new(const char *interface);
int listener_add_handler(struct netlistener_t *listener,
    void (*handler)(void *));
int listener_remove_handler(struct netlistener_t *listener,
    void (*handler)(void *));
int listener_run(const struct netlistener_t *listener);
int listener_stop(struct netlistener_t *listener);
int listener_resume(struct netlistener_t *listener);
int listener_free(struct netlistener_t *listener);

#endif