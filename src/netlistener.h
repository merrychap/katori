#ifndef __NETLISTENER_H__
#define __NETLISTENER_H__

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include <event2/event.h>

typedef enum {
    LISTENER_NULL_PTR    = -1,
    SETTINGS_NULL_PTR    = -2,
    INTERFACE_NULL_PTR   = -3,
    LISTENER_RECV_ERROR  = -4
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

    void (*handler)(void * arg);
};

struct netlistener_t * listener_new(const char *interface);
int listener_run(const struct netlistener_t *listener);
int listener_stop(struct netlistener_t *listener);
int listener_resume(struct netlistener_t *listener);
int listener_free(struct netlistener_t *listener);

#endif