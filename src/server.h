#ifndef _SERVER_H
#define _SERVER_H


#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <pthread.h>

#include "thread_pool.h"


typedef struct server_t        server_t;
typedef struct sniffer_t       sniffer_t;
typedef struct packet_arg_t    packet_arg_t;
typedef struct interface_t     interface_t;
typedef struct user_settings_t user_settings_t;
typedef struct ifaddrs         ifaddrs_t;


typedef enum {
    server_recv_error      = -1,
    sniffer_create_failure = -2,
    server_run_error       = -3,
    server_destroy_error   = -4,
    server_null_exec_error = -5,
    server_null_error      = -6,
    server_exec_join_error = -7
} server_error_t;



struct interface_t {
    char *name;
};


struct user_settings_t {
    int mode;
    interface_t *interface;
    FILE        *logfile;
};


struct server_t {
    int    socket_fd;
    size_t is_online;
    unsigned char *buffer;
    
    pthread_t       exec;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    
    thread_pool_t *tpool;
    sniffer_t     *sniffer;

    FILE * logfile;
};


struct sniffer_t {
    pthread_mutex_t lock;
    
    FILE * logfile;

    size_t icmp;
    size_t tcp;
    size_t udp;
    size_t others;
};


struct packet_arg_t {
    sniffer_t *sniffer;
    unsigned char *buffer;
    size_t size;
};


interface_t *get_all_interfaces(size_t *size);

int remove_interface(interface_t  *interface);
int remove_interfaces(interface_t *interfaces, const size_t size);

server_t * server_create(user_settings_t *settings);
int        server_run(server_t *server);
int        server_destroy(server_t *server);


#endif