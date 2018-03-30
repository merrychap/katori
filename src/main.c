#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "server.h"
#include "cli.h"


int main(int argc, char **argv) {
    user_settings_t *settings = settings_setup();
    if (settings == NULL) {
        fprintf(stderr, "[-] Error while configuring settings.\n");
        return setup_error;
    }

    start_prompt(settings);

    return 0;
}