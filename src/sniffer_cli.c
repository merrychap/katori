#include <stdio.h>
#include <stdlib.h>

#include "sniffer_cli.h"
#include "cli_utils.h"

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
monitor(const struct sniffer_t *sniffer)
{
    if (sniffer == NULL)
        return SNIFFER_NULL_PTR;

    char key = 0;
    // FILE * logfile = sniffer->logfile;

    fprintf(stderr, "[*] Type <q> to back.\n");
    
    term_nonblocking();
    do {
        fprintf(stderr, "TCP : %zu   UDP : %zu   ICMP : %zu   Others : %zu\r",
            sniffer->tcp, sniffer->udp, sniffer->icmp, sniffer->others);
        
        // TODO ADD PACKET PARSING AND LOGGING
        key = getchar();
    } while (1 && key != 'q');
    term_reset();
    
    return 0;
}

static int
prerun(void)
{
    clear_window();

    return 0;
}

int
sniffing_mode(const struct sniffer_t *sniffer)
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
                if (monitor(sniffer) != 0)
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