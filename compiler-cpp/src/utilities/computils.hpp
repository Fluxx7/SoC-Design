#include <limits.h>
#include <stdio.h>
#include <cstddef>
#include <string>
#include <iostream>
#include "classes/array_vector.hpp"
#include "classes/f_string.hpp"
#ifndef linesize
#define linesize 256
#endif
#pragma once
#define here() printf("here\n")

extern char rline_ptr[linesize];
extern int linenum;
extern int truenum;
extern int debug;
extern FILE* mirror;
extern int reflect;
extern int preflect;
extern int verbose;
extern int pverbose;
extern int pronly;
extern struct comp_target* target;
extern FILE* input;
extern FILE* output;
extern FILE* processed_input;
extern FILE* statements;
extern FILE* tokens;


/**
 * Extracts a portion of a string before a specific character sequence
 * @param src Input string to be split
 * @param out String to store output 
 * @param iindex Address of an integer for continuous splitting
 * @param delim The character sequence to terminate the split at
 * @returns 1 if the string was split successfully, 0 if the string had no characters to split
 */
int ssplit(const char* src, char* out, int* iindex, const char* delim);

/**
 * Outputs a string with no spaces
 * @param src Input string to be formatted
 * @param out String to store output 
 */
void sclean(const char* src, char* out);


/**
 * Outputs a string with a specified number of a specified character in between tokens not containing that character
 * @param src Input string to be formatted
 * @param out String to store output 
 * @param character The character being targeted
 * @param spacing The number of characters to put in between non-character tokens (must be >= 0)
 */
void sclean_i(const char* src, char* out, char character, int spacing);

/**
 * Returns 1 if the string 'src' starts with 'match', 0 if it doesn't
 */
int smatch(const char* src, const char* match);

/**
 * returns an integer value equal to the value in number_string
 * or returns -1 if it's invalid 
 */
int parse_number(const char* number_string);

/**
 * returns -1, and prints the input as a formatted string
 */
template <typename... Args>
int compile_error(std::format_string<Args...> fmt, Args&&... args) {
    auto buf = std::format(fmt, std::forward<Args>(args)...);
    if (strlen(rline_ptr) != 0){
        std::println("Error: {}\n-> Line {}: {}\n", buf, truenum, rline_ptr);
    } else {
        std::println("Error: {}\n", buf);
    }
    return 1;
}

/**
 * takes an integer and puts the binary representation into token
 */
int itob(int integer, char* token);



#define s_slice(string, index) (string+index)
#define pmirror(string, tokens) fputs(string, tokens); if(reflect) fputs(string, mirror)