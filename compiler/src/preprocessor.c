#include "preprocessor.h"
#include <stdlib.h>
#include <string.h>

#include "computils.h"

int preprocess(FILE* input, FILE* processed_input, FILE* statements, int reflect, int verbose) {
    char rawline[linesize];
    FILE* state_mirror = fopen("smirror.txt", "w+");
    FILE* proc_mirror = fopen("procmirror.txt", "w+");
    rewind(input);
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
    // statement processing
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        } 
        char line[linesize];
        sclean(rawline, line);
        if (strlen(line) == 0 || line[0] == '#') {
            fputs("blank\n", state_mirror);
        } 
        if (smatch(line, "DEFINE")) {
            fputs("DEF\n", state_mirror);
        } 
        if (smatch(line, "")) {

        }
    }
    char** constants;
    int* const_values;
    int const_count = -1;
    rewind(input);

    // Replacing constants
    while (1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        } 
        char line[linesize];
        sclean(rawline, line);
        
    }
    if (const_count != -1) {
        for (int i = 0; i <= const_count; i++) {
            free(constants[i]);
        }
        free(constants);
        free(const_values);
    }
    fclose(state_mirror);
    fclose(proc_mirror);
    return 0;
}