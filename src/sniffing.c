#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sniffing.h"


#define SLEEP 10000


static void print_sniffing_header() {
    fprintf(stdout, COLOR_CYAN "-->" COLOR_RESET " Sniffing mode.\n");
}


static void print_sniffing_menu() {
    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "   [1] Setting up.\n");
    fprintf(stdout, "   [2] Start sniffer.\n");
    fprintf(stdout, "   [3] Stop sniffer.\n");
    fprintf(stdout, "   [4] Monitor.\n");
    fprintf(stdout, "   [5] Back.\n");
    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "Choice: ");
}


static interface_t * set_interface() {
    size_t size  = 0;
    size_t index = 0;
    interface_t *interface  = (interface_t *) malloc(sizeof(interface_t));
    interface_t *interfaces = get_all_interfaces(&size);

    fprintf(stdout, "List of available interfaces:\n");
    
    for (int i = 0; i < size; i++) {
        printf("    %d. %s\n", i, interfaces[i].name);
    }

    fprintf(stdout, "[*] Enter interface index: ");
    index = input_choice();
    interface->name = interfaces[index].name;

    remove_interfaces(interfaces, size);
    return interface;
}


static int remove_settings(user_settings_t *settings) {
    if (settings) {
        remove_interface(settings->interface);
        free(settings);
    }
    return 0;
}


static user_settings_t * setting_up() {
    user_settings_t * settings = (user_settings_t *) malloc(sizeof(user_settings_t));

    if (settings == NULL) return NULL;

    interface_t *interface = set_interface();
    if (interface == NULL) return NULL;

    // TODO set other params
    settings->interface = interface;
    return settings;
}


static int monitor(server_t *server) {
    if (server == NULL) return null_server_error;

    char key = 0;
    
    fprintf(stdout, "[*] Type <q> to back.\n");
    
    term_nonblocking();
    
    do {
        printf("TCP : %lu   UDP : %lu   ICMP : %lu   Others : %lu\r", server->sniffer->tcp, server->sniffer->udp, server->sniffer->icmp, server->sniffer->others);
        usleep(SLEEP);
        key = getchar();
    } while (1 && key != 'q');
    
    term_reset();
    
    return 0;
}


static int exit_sniffing_mode(server_t *server, user_settings_t *settings) {
    server_destroy(server);
    remove_settings(settings);
    return 0;
}


int sniffing_mode() {
    server_t        *server   = 0;
    user_settings_t *settings = 0;
    
    int choice    = 0;
    int err_code  = 0;
    int exit_flag = 0;

    print_sniffing_header();

    while (!exit_flag) {
        print_sniffing_menu();

        choice = input_choice();
        printf("\n");

        switch (choice) {
            case 1: // Setting up
                settings = setting_up();
                server   = server_create(settings);
                if (server == NULL) fprintf(stderr, "\n[-] Error while configuring the server. Try to use sudo.\n\n");
                break;
            case 2: // Start sniffer
                server_run(server);
                break;
            case 3: // Stop sniffer
                server->is_online = 0;
                break;
            case 4: // Monitor
                if (monitor(server) != 0) fprintf(stderr, "\n[-] Setting up sniffer first!\n");
                break;
            case 5: // Exit
                exit_flag = 1;
                break;
            default:
                print_invalid_option();
                break;
        }
    }

    exit_sniffing_mode(server, settings);
    return err_code;
}