#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sniffing.h"


#define SLEEP 5000


static void print_sniffing_header() {
    fprintf(stdout, COLOR_CYAN "-->" COLOR_RESET " Sniffing mode.\n");
}


static void print_sniffing_menu() {    
    system("clear");

    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "   [1] Change settings.\n");
    fprintf(stdout, "   [2] Start sniffer.\n");
    fprintf(stdout, "   [3] Stop sniffer.\n");
    fprintf(stdout, "   [4] Monitor.\n");
    fprintf(stdout, "   [5] Back.\n");
    fprintf(stdout, "\n=================================\n");
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
    interface_t     *interface = set_interface();

    if (settings == NULL || interface == NULL) return NULL;

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


static int prerun(server_t **server, strbuf_t *strbuf) {
    if (*server != NULL) return server_already_created;
    if (strbuf == NULL) return null_strbuf_error;

    user_settings_t *settings = setting_up();
    *server = server_create(settings);
    if (*server == NULL) add_to_strbuf(strbuf, "\n[-] Error while configuring the server. Try to use sudo.\n\n");

    remove_settings(settings);
    return 0;
}


int sniffing_mode(server_t **_server, strbuf_t *strbuf) {    
    int choice    = 0;
    int err_code  = 0;
    int exit_flag = 0;

    size_t buf_size = 0;

    prerun(_server, strbuf);
    server_t *server = *_server;

    print_sniffing_header();

    while (!exit_flag) {
        print_sniffing_menu();
        print_strbuf(strbuf);

        choice = input_choice();
        printf("\n");

        switch (choice) {
            case 1: // Change settings
                break;
            case 2: // Start sniffer
                server_run(server);
                add_to_strbuf(strbuf, "\n[+] Sniffer started!\n");
                break;
            case 3: // Stop sniffer
                server->is_online = 0;
                add_to_strbuf(strbuf, "\n[+] Sniffer stopped!\n");
                break;
            case 4: // Monitor
                if (monitor(server) != 0) add_to_strbuf(strbuf, "\n[-] Setting up sniffer first!\n");
                break;
            case 5: // Exit
                exit_flag = 1;
                break;
            default:
                print_invalid_option(strbuf);
                break;
        }
    }
    return err_code;
}