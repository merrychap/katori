#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#include "server.h"


#define BUF_SIZE         65536
#define INTERFACES_COUNT 100


struct server_t {
    int socket_fd;
    char *buffer;
};


// don't forget to free this pointer!
char ** get_all_interfaces() {
    size_t index = 0;

    char **inf_names = (char *) malloc(INTERFACES_COUNT * sizeof(char *));
    
    ifaddrs_t *addrs    = 0;
    ifaddrs_t *tmp_addr = 0;

    getifaddrs(&addrs);
    tmp_addr = addrs;

    while (tmp_addr) {
        if (tmp_addr->ifa_addr && tmp_addr->ifa_addr->sa_family == AF_PACKET)
            inf_names[index++] = tmp_addr->ifa_name;
        tmp_addr = tmp_addr->ifa_next;
    }
    
    freeifaddrs(addrs);

    return inf_names;
}


server_t * server_create() {
    server_t *server = (server_t *) malloc(sizeof(server_t));
    
    if (server == NULL) return NULL;

    server->socket_fd = socket(AF_INET, SOCK_RAW, htons(ETH_P_ALL));
    if (server->socket_fd < 0) { server_destroy(server); return NULL; }

    server->buffer = (char *) malloc(BUF_SIZE);

    return server;
}


int server_run(server_t *server) {
    int err_code = 0;

    // loop with handling

    return err_code;
}


int server_destroy(server_t *server) {
    if (server->socket_fd) close(server->socket_fd);
    free(server);
    return 0;
}