#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "cli.h"
#include "server.h"


struct user_settings_t {
    int mode;
};


void * thread_start_server() {
    fprintf(stderr, "[*] Starting a sniffer...\n");
    
    server_t *server = server_create();

    if (server == NULL) {
        fprintf(stderr, "[-] Error occured.\n");
        goto exit_thread;
    }
    
    server_run(server);

exit_thread:
    pthread_exit(NULL);
    return (NULL);
}


user_settings_t * settings_setup() {
    user_settings_t * settings = 0;

    char *interface = set_interface();
    if (interface == NULL) return NULL;

    // TODO set other params

    return NULL;
}


int start_prompt(user_settings_t *settings) {
    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_start_server, NULL) != 0) {
        fprintf(stderr, "[-] Error while starting a sniffer.\n");
        return sniffer_start_error;
    }
    return 0;
}


char * set_interface() {
    char **interfaces = get_all_interfaces();
    free(interfaces);

    return NULL;
}

