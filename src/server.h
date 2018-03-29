#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>

#ifndef _SERVER_H
#define _SERVER_H


typedef struct server_t server_t;
typedef struct ifaddrs  ifaddrs_t;


typedef enum {
    server_recv_error = 1
} server_error_t;


char **get_all_interfaces();
server_t * server_create();
int server_run(server_t *server);
int server_destroy(server_t *server);

#endif