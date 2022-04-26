#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int total_len(const char** lines){
    int res = 0;
    int i = 0;
    while (lines[i]){
        res += strlen(lines[i]);
        i++;
    }
    return res;
}

int total_items(char* src, const char* items){
    int res = 0;
    while (*src != 0x00){
        if (strchr(items, *src)){
            res++;
        }
        src++;
    }
    return res;
}

int _nsplit(char** ssrc, const char* delim){
    char* src = *ssrc;
    char* str = malloc((strlen(src) + 1) * sizeof(char));
    strcpy(str, src);
    char* item = strtok(str, delim);
    int i = 0;
    while (item){
        item = strtok(NULL, delim);
        i++;
    }
    free(str);
    return i;
}

char** split(char** ssrc, const char* delim){
    char* src = *ssrc;
    int items = _nsplit(&src, delim);
    char** tres = (char**)malloc((items + 1) * sizeof(char*));
    char* str = malloc((strlen(src) + 1) * sizeof(char));
    strcpy(str, src);
    char* item = strtok(str, delim);
    int i = 0;
    while (item){
        tres[i] = item;
        item = strtok(NULL, delim);
        i++;
    }
    char** res = (char**)malloc((items + 1) * sizeof(char*));
    res[items] = 0x00;
    for (int i = 0; i < items; i++){
        res[i] = tres[i];
    }
    free(tres);
    return res;
}

char** splitlines(char** ssrc){
    return split(ssrc, "\n");
}

int get_index_of_max_element(int* arr, int n){
    int max = arr[0];
    int res = 0;
    for (int i = 1; i < n; i++) {
        if (max < arr[i]) {
            max = arr[i];
            res = i;
        }
    }
    return res;
}

int get_index_of_min_element(int* arr, int n){
    int min = arr[0];
    int res = 0;
    for (int i = 1; i < n; i++) {
        if (min > arr[i]) {
            min = arr[i];
            res = i;
        }
    }
    return res;
}

int get_index_of_max_min_element(int* arr, int n, bool mode){
    if (mode){
        return get_index_of_max_element(arr, n);
    }
    return get_index_of_min_element(arr, n);
}

int intlen(int num){
    int res = 0;
    if (num == 0){
        res = 1;
    }

    while (num != 0){
        res++;
        num = num / 10;
    }
    return res;
}

char* strrev(char* str){
    char* res = str;
    char ch;
    int len = strlen(str);
    for (int i = 0; i < len/2; i++){
        ch = res[i];
        res[i] = res[len-1-i];
        res[len-1-i] = ch;
    }
    return res;
}

char* itoa(int num){
    int i = 0;
    char* str = malloc((intlen(num) + 1) * sizeof(char));
    int isNegative = 0;
  
    if (num == 0){
        str[i++] = '0';
        str[i] = 0x00;
        return str;
    }

    if (num < 0){
        isNegative = 1;
        num = -num;
    }

    while (num != 0){
        int rem = num % 10;
        str[i++] = rem + 0x30;
        num = num / 10;
    }

    if (isNegative){
        str[i++] = '-';
    }

    str[i] = 0x00;
    str = strrev(str);
    return str;
}

char* uitoa(uint32_t num){
    int i = 0;
    char* str = malloc((intlen(num) + 1) * sizeof(char));
  
    if (num == 0){
        str[i++] = '0';
        str[i] = 0x00;
        return str;
    }

    while (num != 0){
        int rem = num % 10;
        str[i++] = rem + 0x30;
        num = num / 10;
    }

    str[i] = 0x00;
    str = strrev(str);
    return str;
}