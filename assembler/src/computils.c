#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "computils.h"

int ssplit(const char* src, char* out, int* iindex, const char* delim){
    int delim_match = 0;
    int* index;
    int orig_index;
    int output = out == NULL ? 0 : 1;
    if (iindex == NULL) {
        index = (int*) malloc(sizeof(int));
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
                free(index);
            } else {
                *iindex = orig_index;
            }
            return 0;
        } else {
            if (src[*index] != delim[delim_match]){
                if (delim_match > 0) {
                    delim_match = 0;
                }
                //printf("Index: %d, token: %s, line: %s\n", *index, out, src+*index);
                if(output) out[*index-orig_index] = src[*index];
            } else {
                //printf("Index: %d, delim_match: %d, token: %s, line: %s\n", *index, delim_match, out, src+*index);
                delim_match++;
                if (output) out[*index-orig_index] = src[*index];
                if (delim[delim_match] == '\0') {
                    if (output) out[(*index-orig_index)-delim_match+1] = '\0';
                    (*index)++;
                    if (iindex == NULL) {
                        free(index);
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
    int imm;
    if (number_string[0] == '0' && number_string[1] == 'x'){
        if ((imm = strtol(number_string+2, NULL, 16)) > 0) {
            return imm;
        }
    } else if(number_string[0] == '0' && number_string[1] == 'b'){
        char bin_num[strlen(number_string+2)];
        sclean_i(number_string+2, bin_num, '_', 0);
        if ((imm = strtol(bin_num, NULL, 2)) > 0) {
            return imm;
        }
    } else if ((imm = atoi(number_string)) > 0){
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

int compile_error(const char * message, ...){
    char buf[1024];
    va_list args;
    va_start(args, message);
    vsprintf(buf, message, args);
    if (strlen(rline_ptr) != 0){
        printf("Error: %s\n-> Line %d: %s\n", buf, truenum, rline_ptr);
    } else {
        printf("Error: %s\n", buf);
    }
    return 1;
}