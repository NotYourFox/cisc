#ifndef ARGPARSER_H
#define ARGPARSER_H

struct arguments{
    char* args[2];
    int maxlength;
    bool format;
};

struct arguments arg_parse(int argc, char** argv);

#endif