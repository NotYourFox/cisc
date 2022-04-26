#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

#include "trycatch/trycatch.h"
#include "utils/utils.h"
#include "argparser/argparser.h"

FILE* sfopen(const char* fname, const char* mode){
    FILE* fp = fopen(fname, mode);
    if (!fp){
        throw(EIO);
    }
    return fp;
}

struct entry {
    const char* id;
    bool isComplementary;
    int starting_pos;
    int ending_pos;
    const char* name;
    const char* sequence;
};

struct entry* entries;

int satoi(const char* str){
    int res = atoi(str);
    if (res == 0 && str != "0"){
        throw(EINVAL);
    }
    return res;
}

int sfprintf(FILE* fp, const char* format, ...){
    int res = 0;
    va_list argptr;
    va_start(argptr, format);
    res = vfprintf(fp, format, argptr);
    if (res < 0){
        throw(res);
    }
    return res;
}

char* sequence_lines_to_proper_string(const char** src){
    char* res = malloc((total_len(src) + 1) * sizeof(char));
    strcpy(res, src[0]);
    int i = 1;
    while (src[i]){
        if (src[i][0] == '>'){
            break;
        }
        res = strcat(res, src[i]);
        i++;
    }
    return res;
}

struct entry* parse_entry(const char** src){
    char** str = (char**)src;
    char* header = *str;
    char** raw_parts = split(&header, ":-");
    if (_nsplit(&header, ":-") != 3){
        throw(EINVAL);
    }
    char** parts = malloc(sizeof(char*) * 6);
    parts[0] = raw_parts[0] + 1; //id
    if (isalpha(raw_parts[1][0]) && raw_parts[1][0] != 'c'){
        throw(EINVAL);
    }
    parts[1] = raw_parts[1][0] == 'c' ? "c" : ""; //"c" for complementary
    parts[2] = raw_parts[1] + (raw_parts[1][0] == 'c'); //Actual starting pos
    parts[3] = malloc(strchr(raw_parts[2], ' ') - raw_parts[2] + 1);
    parts[4] = malloc(strchr(raw_parts[2], 0x00) - strchr(raw_parts[2], ' ') + 1);
    strncpy(parts[3], raw_parts[2], strchr(raw_parts[2], ' ') - raw_parts[2]); //Actual ending pos
    strcpy(parts[4], strchr(raw_parts[2], ' ') + 1); //Name
    parts[5] = sequence_lines_to_proper_string(src + 1);
    int ex;
    int start;
    int end;
    try {
        start = satoi(parts[2]);
        end = satoi(parts[3]);
    } catch(ex) {
        throw(ex);
        return 0;
    }
    if ((start > end && strcmp(parts[1], "c")) || (start < end && !strcmp(parts[1], "c"))){
        throw(EINVAL);
    }
    struct entry* entry = malloc(sizeof(struct entry));
    entry->id = parts[0];
    entry->isComplementary = parts[1] == "c";
    entry->starting_pos = start;
    entry->ending_pos = end;
    entry->name = parts[4];
    entry->sequence = parts[5];
    return entry;
}

void check_complementarity(struct entry* entries, int num){
    bool comp = entries[0].isComplementary;
    for (int i = 1; i < num; i++){
        if (entries[i].isComplementary != comp){
            throw(EINVAL);
        }
    }
}

int get_total_entry_matches_by_id(struct entry* entries, const char* id, int num){
    int n = 0;
    for (int i = 0; i < num; i++){
        if (!strcmp(entries[i].id, id)){
            n++;
        }
    }
    return n;
}

char** get_all_entry_ids(struct entry* entries, int num){
    char** res = (char**)malloc(sizeof(char*));
    res[0] = (char*)entries[0].id;
    int n = 1;
    bool x;
    for (int i = 1; i < num; i++){
        isinarray(res, entries[i].id, n, x);
        if (!x){
            res = (char**)realloc(res, (n + 1) * sizeof(char*));
            res[n] = (char*)entries[i].id;
            n++;
        }
    }
    return res;
}

int get_total_num_of_entry_ids(struct entry* entries, int num){
    char** res = (char**)malloc(sizeof(char*));
    res[0] = (char*)entries[0].id;
    int n = 1;
    bool x;
    for (int i = 1; i < num; i++){
        isinarray(res, entries[i].id, n, x);
        if (!x){
            res = (char**)realloc(res, (n + 1) * sizeof(char*));
            res[n] = (char*)entries[i].id;
            n++;
        }
    }
    free(res);
    return n;
}

struct entry* get_entries_subarray_by_id(struct entry* entries, const char* id, int num){
    struct entry* res = malloc(get_total_entry_matches_by_id(entries, id, num) * sizeof(struct entry));
    int j = 0;
    for (int i = 0; i < num; i++){
        if (!strcmp(entries[i].id, id)){
            res[j] = entries[i];
            j++;
        }
    }
    return res;
}

struct entry* sort_entries(struct entry* entries, int num){
    struct entry* res = malloc(num * sizeof(struct entry));
    int* pos = malloc(num * sizeof(int));
    int ex;
    try {
        check_complementarity(entries, num);
    } catch(ex) {
        raise(ex, "\n%s: complementarity conflict detected. Aborting.\n", entries[0].id);
    }
    for (int i = 0; i < num; i++){
        pos[i] = entries[i].starting_pos;
    }
    res[0] = entries[0];
    int idx;
    for (int i = 0; i < num; i++){
        idx = get_index_of_max_min_element(pos, num - i, entries[0].isComplementary);
        res[i] = entries[idx];
        remove_element(pos, idx, num - i, int);
        remove_element(entries, idx, num - i, struct entry);
    }
    return res;
}

char* format_sequence(const char* sequence){
    int num = strlen(sequence) + 1 + strlen(sequence) / 80 + 1;
    char* res = malloc((num + 1) * sizeof(char));
    strcpy(res, sequence);
    char* tmp = malloc((strlen(sequence) + 1 + strlen(sequence) / 80 + 1) * sizeof(char));
    char* bak = tmp;
    strcpy(tmp, sequence);
    int i = 0;
    while (strlen(tmp) > 80){
        i += 80;
        insert_item(res, '\n', i, num);
        num += 1;
        i += 1;
        tmp += 80;
    }
    res = strcat(res, "\n");
    free(bak);
    return res;
}

void format_output_header(char* header){
    char* tmp = header;
    char* sym = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_>\n";
    while (*header != 0x00){
        if (!strchr(sym, *header)){
            *header = '_';
        }
        header++;
    }
    header = tmp;
}

char* get_sequence(struct entry entry, int hstart, int hend, const char* sequence, bool format){
    const char* id = entry.id;
    bool comp = entry.isComplementary;
    char* compc = comp ? "c" : "";
    const char* name = entry.name;
    char* header = malloc((strlen(id) + strlen(name) + strlen(compc) + intlen(hstart) + intlen(hend) + 8 + 1 + 1 + 1 + 1) * sizeof(char));
    strcpy(header, ">");
    header = strcat(header, id);
    header = strcat(header, ":range ");
    header = strcat(header, compc);
    header = strcat(header, itoa(hstart));
    header = strcat(header, "-");
    header = strcat(header, itoa(hend));
    header = strcat(header, " ");
    header = strcat(header, name);
    header = strcat(header, "\n");
    if (format){
        format_output_header(header);
    }
    int len = strlen(header) + strlen(sequence) + strlen(sequence) / 80 + 1 + 1;

    char* res = malloc(len * sizeof(char));
    strcpy(res, header);
    res = strcat(res, format_sequence(sequence));
    return res;
}

void clear_symbols(int n){
    usleep(200000);
    for (int i = 0; i < n; i++){
        printf("\b \b");
    }
}

int main(int argc, char** argv){
    struct arguments arguments = arg_parse(argc, argv);
    const char* infile = arguments.args[0];
    const char* outfile = arguments.args[1];
    printf("INPUT:      %s", infile);
    printf("\nOUTPUT:     %s", outfile);
    printf("\nMAX LENGTH: %i", arguments.maxlength);
    printf("\nFORMAT:     %s", arguments.format ? "TRUE" : "FALSE");
    printf("\n");

    FILE* fp;
    int ex;
    try {
        fp = sfopen(outfile, "w");
    } catch(ex) {
        raise(ex, "Could not create file: %s\n", outfile);
    }
    try_s {
        fp = sfopen(infile, "r");
    } catch(ex) {
        raise(ex, "Could not open file: %s\n", infile);
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* str = malloc((fsize + 1) * sizeof(char));
    fread(str, fsize, 1, fp);
    fclose(fp);
    char** lines = splitlines(&str);
    entries = (struct entry*)malloc(sizeof(struct entry) * total_items(str, ">"));
    memset(entries, 0, sizeof(struct entry) * total_items(str, ">"));
    int j = 0;
    for (int i = 0; i < total_items(str, "\n"); i++){
        if (lines[i][0] == '>'){
            printf("\nFound sequence header in line %i.\n", i);
            try {
                entries[j] = *parse_entry((const char**)(lines + i));
            } catch(ex) {
                raise(ex, "Invalid header in line %i. Aborting.\n", i);
            }
            printf("ID: %s", entries[j].id);
            printf("\nComplementary: %s", entries[j].isComplementary == true ? "TRUE" : "FALSE");
            printf("\nStarting position: %i", entries[j].starting_pos);
            printf("\nEnding position: %i", entries[j].ending_pos);
            printf("\nName: %s", entries[j].name);
            printf("\nSequence: %s\n", entries[j].sequence);
            j++;
        }
    }
    if (!entries[0].id){
        raise(EOF, "No sequence headers in file. Aborting.\n");
    }

    int num = total_items(str, ">");
    char** ids = get_all_entry_ids(entries, num);
    char* sequence;
    int header_range_start = 0;
    int header_range_end = 0;
    int complementarity = 0;
    int sequence_len = 0;
    int header_num = 1;
    int local_header_num = 1;
    char* res;
    int toclear = 0;
    FILE* out;
    printf("\nConcatenating slices...\n");
    for (int i = 0; i < get_total_num_of_entry_ids(entries, num); i++){
        local_header_num = 1;
        struct entry* local_entries = get_entries_subarray_by_id(entries, ids[i], num);
        local_entries = sort_entries(local_entries, get_total_entry_matches_by_id(entries, ids[i], num));
        header_range_start = local_entries[0].starting_pos;
        complementarity = local_entries[0].isComplementary ? -1 : 1;
        sequence_len = 0;
        sequence = malloc(sizeof(char));
        strcpy(sequence, "");
        for (int ii = 0; ii < get_total_entry_matches_by_id(entries, ids[i], num) - 1; ii++){
            clear_symbols(toclear);
            printf("Processing header %i of %i, entry %i of %i (id: %s, output sequence: %i (local: %i))...", ii, get_total_entry_matches_by_id(entries, ids[i], num), i + 1, get_total_num_of_entry_ids(entries, num), ids[i], header_num, local_header_num);
            toclear = 18 + intlen(ii) + 4 + intlen(get_total_entry_matches_by_id(entries, ids[i], num)) + 8 + intlen(i) + 4 + intlen(num) + 6 + strlen(ids[i]) + 19 + intlen(header_num) + 9 + intlen(local_header_num) + 5;
            sequence_len += strlen(local_entries[ii].sequence);
            sequence = realloc(sequence, (sequence_len + 1) * sizeof(char));
            sequence = strcat(sequence, local_entries[ii].sequence);
            if (complementarity * (local_entries[ii+1].starting_pos - local_entries[ii].ending_pos) > arguments.maxlength){
                header_range_end = local_entries[ii].ending_pos;
                res = get_sequence(local_entries[ii], header_range_start, header_range_end, sequence, arguments.format);
                try {
                    out = sfopen(outfile, "a");
                    sfprintf(out, "%s", res);
                    fclose(out);
                } catch(ex) {
                    raise(ex, "I/O error on output to file: %s. Aborting.\n", outfile);
                }
                header_range_start = local_entries[ii+1].starting_pos;
                free(sequence);
                sequence_len = 0;
                sequence = malloc(sizeof(char));
                strcpy(sequence, "");
                local_header_num++;
            }
        }
        clear_symbols(toclear);
        printf("Processing header %i of %i, entry %i of %i (id: %s, output sequence: %i (local: %i))...", get_total_entry_matches_by_id(entries, ids[i], num), get_total_entry_matches_by_id(entries, ids[i], num), i+1, get_total_num_of_entry_ids(entries, num), ids[i], header_num, local_header_num);
        toclear = 18 + intlen(get_total_entry_matches_by_id(entries, ids[i], num)) + 4 + intlen(get_total_entry_matches_by_id(entries, ids[i], num)) + 8 + intlen(i) + 4 + intlen(num) + 6 + strlen(ids[i]) + 19 + intlen(header_num) + 9 + intlen(local_header_num) + 5;
        sequence_len += strlen(local_entries[get_total_entry_matches_by_id(entries, ids[i], num)-1].sequence);
        sequence = realloc(sequence, (sequence_len + 1) * sizeof(char));
        sequence = strcat(sequence, local_entries[get_total_entry_matches_by_id(entries, ids[i], num)  -1].sequence);
        header_range_end = local_entries[get_total_entry_matches_by_id(entries, ids[i], num) - 1].ending_pos;
        res = get_sequence(local_entries[get_total_entry_matches_by_id(entries, ids[i], num) - 1], header_range_start, header_range_end, sequence, arguments.format);
        try {
            out = sfopen(outfile, "a");
            sfprintf(out, "%s", res);
            fclose(out);
        } catch(ex) {
            raise(ex, "I/O error on output to file: %s. Aborting.\n", outfile);
        }
        free(sequence);
        header_num++;
    }
    printf("\nDone.\n");
    return 0;
}