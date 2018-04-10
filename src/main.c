#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "server.h"
#include "cli.h"


int main(int argc, char **argv) {
    int err_code = start_prompt();

    if (err_code == setup_error)
        fprintf(stderr, "[-] Error while configuring settings.\n");
    
    if (err_code == sniffer_start_error)
        fprintf(stderr, "[-] Error while starting a sniffer.\n");
    
    return 0;
}