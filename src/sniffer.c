#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sniffer.h"
#include "utils.h"

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

static int
monitor(void)
{
    // if (server == NULL) return null_server_error;

    // char key       = 0;
    // FILE * logfile = server->sniffer->logfile;

    // fprintf(stdout, "[*] Type <q> to back.\n");
    
    // term_nonblocking();
    // do {
    //     printf("TCP : %lu   UDP : %lu   ICMP : %lu   Others : %lu\r", server->sniffer->tcp, server->sniffer->udp, server->sniffer->icmp, server->sniffer->others);
        
    //     // TODO ADD PACKET PARSING AND LOGGING
        
    //     usleep(SLEEP);
    //     key = getchar();
    // } while (1 && key != 'q');
    // term_reset();
    
    return 0;
}

static int
prerun(void)
{
    clear_window();

    return 0;
}

struct sniffer_t *
sniffer_new(FILE *logfile)
{
    if (logfile == NULL)
        return NULL;
    
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
sniffer_free(struct sniffer_t *sniffer)
{
    if (sniffer == NULL)
        return SNIFFER_NULL_PTR;
    
    free(sniffer);
    
    return 0;
}

int
sniffing_mode(const struct katori_t *katori)
{
    int choice    = 0;
    int err_code  = 0;
    int exit_flag = 0;

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
                run("Sniffer started!");
                break;
            case 3: // Stop sniffer
                // server->is_online = 0;
                good("Sniffer stopped!");
                break;
            case 4: // Monitor
                if (monitor() != 0)
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