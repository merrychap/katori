#include <stdio.h>
#include <stdlib.h>
#include "server.h"


int main(int argc, char **argv) {
    fprintf(stderr, "[*] Staring a sniffer...\n");
    
    server_t *server = server_create();

    if (server == NULL) {
        fprintf(stderr, "[-] Error occured.\n");
        exit(1);
    }
    
    server_run(server);

    return 0;
}