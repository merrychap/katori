#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "sniffing.h"


void print_sniffing_header() {
    fprintf(stdout, COLOR_CYAN "-->" COLOR_RESET " Sniffing mode.\n");
}


void print_sniffing_menu() {
    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "   [1] Setting up.\n");
    fprintf(stdout, "   [2] Start sniffer.\n");
    fprintf(stdout, "   [3] Stop sniffer.\n");
    fprintf(stdout, "   [4] Monitor.\n");
    fprintf(stdout, "   [5] Back.\n");
    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "Choice: ");
}


interface_t * set_interface() {
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


int remove_interface(interface_t *interface) {
    interface->name = 0;
    free(interface);
    return 0;
}


int remove_settings(user_settings_t *settings) {
    if (settings) {
        remove_interface(settings->interface);
        free(settings);
    }
    return 0;
}


user_settings_t * setting_up() {
    user_settings_t * settings = (user_settings_t *) malloc(sizeof(user_settings_t));

    if (settings == NULL) return NULL;

    interface_t *interface = set_interface();
    if (interface == NULL) return NULL;

    // TODO set other params
    settings->interface = interface;
    return settings;
}


int monitor(server_t *server) {
    fprintf(stdout, "[*] Type Ctrl+D to back.\n");
    
    while (getchar() != EOF) {
        fprintf(stdout, "[*] Some debug output\n");
    }
    return 0;
}


int exit_sniffing_mode(server_t *server, user_settings_t *settings) {
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
                if (server == NULL) fprintf(stderr, "\n[-] Error while configuring the server. Try to user sudo.\n\n");
                break;
            case 2: // Start sniffer
                server_run(server);
                break;
            case 3: // Stop sniffer
                server_destroy(server);
                break;
            case 4: // Monitor
                monitor(server);
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