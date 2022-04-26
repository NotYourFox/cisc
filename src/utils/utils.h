#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>

int total_len(const char** lines);
int total_items(char* src, const char* items);
int _nsplit(char** ssrc, const char* delim);
char** split(char** ssrc, const char* delim);
char** splitlines(char** ssrc);
int get_index_of_max_element(int* arr, int n);
int get_index_of_min_element(int* arr, int n);
int get_index_of_max_min_element(int* arr, int n, bool mode);
int intlen(int num);
char* strrev(char* str);
char* itoa(int num);
char* uitoa(uint32_t num);

#define remove_element(_arr, _pos, _num, _type) if (_pos >= _num+1){throw(EINVAL);}else{_type* _res = (_type*)malloc((_num - 1) * sizeof(_type));for (int _i = _pos; _i < _num - 1; _i++){_arr[_i] = _arr[_i+1];}memcpy(_res, _arr, (_num - 1) * sizeof(_type));free(_arr);_arr = _res;}
#define isinarray(_arr, _item, _num, _res) _res = false;for (int _i = 0; _i < _num; _i++){if(_arr[_i] == _item){_res = true;}else{if(!strcmp(_arr[_i], _item)){_res = true;}}}
#define insert_item(_arr, _val, _pos, _num) for(int _i = _num - 1; _i >= _pos - 1; _i--){_arr[_i+1]=_arr[_i];}_arr[_pos-1]= _val;

#endif