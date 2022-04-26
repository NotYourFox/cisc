#include <argp.h>
#include <stdbool.h>
#include <stdlib.h>
#include "argparser.h"

#define MAX_LENGTH_SHORT 'm'
#define FORMAT_OUTPUT_HEADER_SHORT -1

const char *argp_program_version = "CSC v.1.0-15";
const char *argp_program_bug_address = "<lutzy.yuly@yandex.ru>";
static char doc[] = "Chromosome slice concatenation utility.\nDistributed open-source under GNU GPL v3 (see: https://www.gnu.org/licenses/)\nSource repository: \n© Copyright Svyatoslav Matveenko (NotYourFox), April 2022. All rights reserved.";
static char args_doc[] = "INFILE OUTFILE";
static struct argp_option options[] = { 
    {"maxlength", MAX_LENGTH_SHORT, "MAXLENGTH", 0, "Max length of a chromosome slice (optional, default 100000)"},
    {"format", FORMAT_OUTPUT_HEADER_SHORT, 0, OPTION_ARG_OPTIONAL, "Format the output header and sequence (optional)"},
    {0} 
};

static error_t parse_opt(int key, char *arg, struct argp_state *state){
    struct arguments* arguments = state->input;
    switch (key){
        case MAX_LENGTH_SHORT:
            arguments->maxlength = atoi(arg);
            break;
        case FORMAT_OUTPUT_HEADER_SHORT:
            arguments->format = true;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num > 4){
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2){
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

struct arguments arg_parse(int argc, char** argv){
    struct arguments arguments;
    arguments.maxlength = 100000;
    arguments.format = false;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    return arguments;
}