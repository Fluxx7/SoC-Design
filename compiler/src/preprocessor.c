#include "preprocessor.h"
#include <stdlib.h>
#include <string.h>

#include "computils.h"

int preprocess(FILE* input, FILE* processed_input, FILE* statements, int reflect, int verbose) {
    char rawline[linesize];
    /* 
    two goals: 
    - break down each line into predefined statements
        - outline the statements found in the statements file to make later steps faster
        - this is also needed to safely replace labels, macros, and constants
    - replace all labels, macros, and constants with the equivalent values, and remove comments
        - start with macros, as they can include other macros, labels, and constants, and they will also affect the address of labels
            - loop through and expand each macro found
                - can loop repeatedly and track located macros until no more macros are found
                - better option might be to recursively expand a macro until the macro is fully expanded
            - once no more macros exist, replace all constants with predefined values
            - replace all labels with correct line addresses
    
    Final result should only include assembly instructions that can easily be processed by the primary system
    Support both mirroring and verbosity setttings
    */
    char** constants;
    int* const_values;
    int const_count = -1;
    rewind(input);
    while (1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        } 
        char* line = sclean(rawline);
        if (smatch(line, "DEFINE")) {
            int const_index = 6;
            char* const_name = ssplit(line, &const_index, " ");
            char* const_val = sclean(s_slice(line, const_index));
            if (const_count == -1) {
                constants = (char**) malloc(sizeof(char*));
                const_values = (int*) malloc(sizeof(int));
            } else {
                constants = (char**) realloc(constants, sizeof(char*)*(const_count+2));
                const_values = (int*) realloc(const_values, sizeof(int)*(const_count+2));
            }
            const_count++;
            constants[const_count] = const_name;
            int imm;
            if ((imm = parse_number(const_val)) != -1) {
                const_values[const_count] = imm;
                if (verbose) printf("Constant '%s' defined as %d\n", constants[const_count], const_values[const_count]); 
            }
            free(const_val);
        } 
        free(line);
    }
}