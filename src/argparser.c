#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <argp.h>

#include "argparser.h"
#include "utils.h"
#include "log.h"

const char *argp_program_version = "katori 1.0";

struct arguments {
    int verbose;         /* The -v flag */
    char *logfile;       /* log file */
};

static struct argp_option options[] = {
    { "verbose",   'v', 0,            0, "produce verbose output" },
    { "interface", 'i', "INTERFACE",  0, "interface for sniffer listening" },
    { "logfile",   'l', "LOGFILE",    0, "file for logging traffic packets" },
    { 0 }
};

static char args_doc[] = "";

static char doc[] = "A program that provides tools to work with network.";

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    char *pEnd = NULL;
    
    struct arguments *arguments = state->input;

    switch (key) {
        case 'v':
            arguments->verbose = 1;
            break;
        case 'l':
            arguments->logfile = arg;
            break;
        case ARGP_KEY_ARG:
            // if (state->arg_num >= 2) {
	        //     argp_usage(state);
            // }
            break;
        case ARGP_KEY_END:
            // if (state->arg_num < 2) {
	        //     argp_usage(state);
	        // }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int
parse_args(const int argc, char **argv,
    struct katori_config_t *config)
{
    struct arguments arguments;
    FILE *outstream = NULL;

    arguments.verbose = 0;
    arguments.logfile = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.logfile)
        return LOGFILE_NULL_PTR;

    outstream = fopen(arguments.logfile, "w");

    if (outstream == NULL)
        return LOGFILE_FOPEN_ERROR;

    config->verbose = arguments.verbose;
    config->logfile = outstream;

    return 0;
}