#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "computils.hpp"

int ssplit(const char* src, char* out, int* iindex, const char* delim){
    int delim_match = 0;
    int* index;
    int orig_index;
    int output = out == NULL ? 0 : 1;
    if (iindex == NULL) {
        index = new int;
        *index = 0;
        orig_index = 0;
    } else {
        index = iindex;
        if (*index >= strlen(src)){
            return 0;
        }
        orig_index = *iindex;
        //printf("Index: %d\n", *index);
    }
    while(1){
        if (src[*index] == '\0') {
            if(iindex == NULL){
                delete(index);
            } else {
                *iindex = orig_index;
            }
            return 0;
        } else {
            if (src[*index] != delim[delim_match]){
                if (delim_match > 0) {
                    delim_match = 0;
                }
                if(output) out[*index-orig_index] = src[*index];
            } else {
                delim_match++;
                if (output) out[*index-orig_index] = src[*index];
                if (delim[delim_match] == '\0') {
                    if (output) out[(*index-orig_index)-delim_match+1] = '\0';
                    (*index)++;
                    if (iindex == NULL) {
                        delete(index);
                    }
                    return 1;
                } 
            }
            (*index)++;
        }
    }
    
}

void sclean(const char* src, char* out){
    return sclean_i(src, out, ' ', 0);   
}

void sclean_i(const char* src, char* out, char character, int count){
    int index = 0, tok_index = 0;
    int add_char = 0;
    while(1){
        if (src[index] == '\0' || src[index] == '\n') {
            out[tok_index] = '\0';

            //printf("Index: %d, src: %s, final token: %s\n", index, src, token);
            return;
        } else {
            if (src[index] != character){
                if (add_char == 0){
                    add_char = 1;
                }
                //printf("Index: %d, token: %s, line: %s\n", index, token, src+index);
                out[tok_index] = src[index];
                tok_index++;
            } else {
                //printf("Index: %d, token: %s, line: %s\n", index, token, src+index);
                if (add_char == 1){
                    for (int i = 0; i < count; i++){
                        out[tok_index] = character;
                        tok_index++;
                    }
                    add_char = 0;
                }
            }
            index++;
        }
    }
    
}

int smatch(const char* src, const char* match){
    for (int i = 0; i < strlen(match); i++){
        if (src[i] != match[i]) {
            return 0;
        }
    }
    return 1;
}

int parse_number(const char* number_string) {
    f_string n_string = number_string;
    int imm;
    if (n_string[0] == '0' && n_string[1] == 'x'){
        if ((imm = strtol(n_string+2, NULL, 16)) > 0) {
            return imm;
        }
    } else if(n_string[0] == '0' && n_string[1] == 'b'){
        f_string bin_num = n_string.slice(2).sclean_i('_', 0);
        if ((imm = strtol(bin_num, NULL, 2)) > 0) {
            return imm;
        }
    } else if ((imm = atoi(n_string)) > 0 || strcmp(n_string, "0") == 0){
       return imm;
    }
    return -1;
}

int itob(int integer, char* token)
{
    int token_index = 0;
    int i = 15;
    while(i--) {
        token[token_index] = ('0' + ((integer >> i) & 1)); 
        token_index++;
    }
    token[token_index] = '\0';
    return 0;
}

int itob(int integer, f_string& token)
{
    char* token_str = new char[256];
    int token_index = 0;
    int i = 15;
    while(i--) {
        token_str[token_index] = ('0' + ((integer >> i) & 1)); 
        token_index++;
    }
    token_str[token_index] = '\0';
    token = token_str;
    return 0;
}

