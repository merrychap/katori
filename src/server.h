#ifndef _SERVER_H
#define _SERVER_H


#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>


typedef struct server_t        server_t;
typedef struct sniffer_t       sniffer_t;
typedef struct interface_t     interface_t;
typedef struct user_settings_t user_settings_t;
typedef struct ifaddrs         ifaddrs_t;


struct interface_t {
    char *name;
};


struct user_settings_t {
    int mode;
    interface_t *interface;
};


typedef enum {
    server_recv_error      = -1,
    sniffer_create_failure = -2
} server_error_t;


interface_t *get_all_interfaces(size_t *size);

int remove_interfaces(interface_t *interfaces, const size_t size);

server_t * server_create(user_settings_t *settings);

int server_run(server_t *server);

int server_destroy(server_t *server);


#endif