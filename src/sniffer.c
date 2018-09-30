#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sniffing.h"

#define SLEEP           5000
#define MAX_LOGFILE_LEN 1000

static void
print_sniffing_header(void)
{
    fprintf(stdout, COLOR_CYAN "-->" COLOR_RESET " Sniffing mode.\n");
}

static void
print_sniffing_menu(void) {
    clear_window();

    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "   [1] Change settings.\n");
    fprintf(stdout, "   [2] Start sniffer.\n");
    fprintf(stdout, "   [3] Stop sniffer.\n");
    fprintf(stdout, "   [4] Monitor.\n");
    fprintf(stdout, "   [5] Back.\n");
    fprintf(stdout, "\n=================================\n");
}

static interface_t *
set_interface(void)
{
    size_t size  = 0;
    size_t index = 0;
    interface_t *interface  = (interface_t *) malloc(sizeof(interface_t));
    interface_t *interfaces = get_all_interfaces(&size);

    fprintf(stdout, "List of available interfaces:\n");
    
    for (int i = 0; i < size; i++) {
        printf("    %d. %s\n", i, interfaces[i].name);
    }

    while (1) {
        fprintf(stdout, "[*] Enter interface index: ");
        index = input_choice();
        if (index < size) {
            interface->name = interfaces[index].name;
            break;
        }
        fprintf(stderr, "[-] Incorrect choice.\n");
    }

    remove_interfaces(interfaces, size);
    return interface;
}

// static user_settings_t *
// setting_up(void)
// {
//     user_settings_t * settings = (user_settings_t *) malloc(sizeof(user_settings_t));
//     interface_t     *interface = set_interface();

//     if (settings == NULL || interface == NULL) return NULL;

//     settings->interface = interface;
//     settings->logfile   = specify_logfile();

//     return settings;
// }

static int
monitor(server_t *server)
{
    if (server == NULL) return null_server_error;

    char key       = 0;
    FILE * logfile = server->sniffer->logfile;

    fprintf(stdout, "[*] Type <q> to back.\n");
    
    term_nonblocking();
    do {
        printf("TCP : %lu   UDP : %lu   ICMP : %lu   Others : %lu\r", server->sniffer->tcp, server->sniffer->udp, server->sniffer->icmp, server->sniffer->others);
        
        // TODO ADD PACKET PARSING AND LOGGING
        
        usleep(SLEEP);
        key = getchar();
    } while (1 && key != 'q');
    term_reset();
    
    return 0;
}

static int
prerun(void)
{
    clear_window();
    user_settings_t *settings = setting_up();
    *server = server_create(settings);
    if (*server == NULL) {
        bad("Error while configuring the server!");
        fprintf(stderr, "    It can be caused because of several things:\n");
        fprintf(stderr, "      \033[1m[1]\033[22m Bad path of logfile.\n");
        fprintf(stderr, "      \033[1m[2]\033[22m Low priviledges. Try to use \033[1msudo\033[22m in this case.\n\n");
        
    }

    return 0;
}

struct sniffer_t *
sniffer_new(FILE *logfile)
{
    if (logfile == NULL)
        return LOGFILE_NULL_PTR;
    
    struct sniffer_t *sniffer = xmalloc_0(sizeof(struct sniffer_t));

    if (sniffer == NULL)
        return NULL;

    sniffer->logfile = logfile;
    sniffer->icmp    = 0;
    sniffer->tcp     = 0;
    sniffer->udp     = 0;
    sniffer->others  = 0;
    
    return sniffer;
}

int
sniffer_del(struct sniffer_t *sniffer)
{
    if (sniffer == NULL)
        return SNIFFER_NULL_PTR;
    
    free(sniffer);
    
    return 0;
}

int
sniffing_mode(void)
{
    int choice    = 0;
    int err_code  = 0;
    int exit_flag = 0;

    size_t buf_size = 0;

    prerun();

    print_sniffing_header();

    while (!exit_flag) {
        print_sniffing_menu();

        choice = input_choice();
        fprintf(stderr, "\n");

        switch (choice) {
            case 1: // Change settings
                break;
            case 2: // Start sniffer
                server_run(server);
                run("Sniffer started!");
                break;
            case 3: // Stop sniffer
                server->is_online = 0;
                good("Sniffer stopped!");
                break;
            case 4: // Monitor
                if (monitor(server) != 0)
                    bad("Setting up sniffer first!");
                break;
            case 5: // Exit
                exit_flag = 1;
                break;
            default:
                bad("Invalid option.");
                break;
        }
    }
    return err_code;
}