#include <limits.h>
#ifndef linesize
#define linesize 256
#endif

extern char rline_ptr[linesize];
extern int linenum;
extern FILE* mirror;


/**
 * Extracts a portion of a string before a specific character sequence
 * @param src Input string to be split
 * @param iindex Address of an integer for continuous splitting
 * @param delim The character sequence to terminate the split at
 * @returns Split string if the string was split successfully, NULL if splitting sequence was not found
 */
int ssplit(const char* src, char* out, int* iindex, const char* delim);

/**
 * Outputs a string with no spaces
 * @param src Input string to be formatted
 * @returns Address of the output formatted string
 */
int sclean(const char* src, char* out);


/**
 * Outputs a string with a specified number of a specified character in between tokens not containing that character
 * @param src Input string to be formatted
 * @param character The character being targeted
 * @param spacing The number of characters to put in between non-character tokens (must be >= 0)
 * @returns Address of the output formatted string
 */
int sclean_i(const char* src, char* out, char character, int spacing);

int smatch(const char* src, const char* match);

int parse_number(const char* number_string);

int compile_error(const char*, ...);

int itob(int integer, char* token);


#define s_slice(string, index) (string+index)
#define pmirror(string, tokens) fputs(string, tokens); if(reflect) fputs(string, mirror)