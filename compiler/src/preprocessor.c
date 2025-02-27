#include "preprocessor.h"
#include <stdlib.h>
#include <string.h>

#include "computils.h"
#define stmirror(string, file) fputs(string, file); if(mirror) fputs(string, state_mirror)
#define prmirror(string, file) fputs(string, file); if(mirror) fputs(string, proc_mirror)



int preprocess(struct comp_target* target, FILE* input, FILE* processed_input, FILE* statements, int mirror, int verbose) {
    char rawline[linesize];
    FILE* state_mirror; 
    FILE* proc_mirror; 
    if(mirror) {
        state_mirror = fopen("smirror.txt", "w+");
        proc_mirror = fopen("procmirror.txt", "w+");
    }
    if(verbose) printf("Preprocessor start\n");
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
    char** constants;
    int* const_values;
    int const_count = -1;
    int linenum = 0;
    if(verbose) printf("Finding statements and constants...\n");
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        char line[linesize];
        sclean(rawline, line);
        if (strlen(line) == 0 || line[0] == '#') {
            stmirror("blank\n", statements);
        } else if (smatch(rawline, "DEFINE ")) {
            stmirror("blank\n", statements);
            int const_index = strlen("DEFINE ");
            const_count++;
            if (const_count == 0) {
                constants = (char**) malloc(sizeof(char*));
                const_values = (int*) malloc(sizeof(int));
            } else {
                constants = (char**) realloc(constants, sizeof(char*)*(const_count+1));
                const_values = (int*) realloc(const_values, sizeof(int)*(const_count+1));
            } 
            constants[const_count] = (char*) calloc(linesize, linesize*sizeof(char));
            ssplit(rawline, constants[const_count], &const_index, " ");
            char const_val[strlen(s_slice(rawline, const_index))];
            sclean(s_slice(rawline, const_index), const_val);
            int imm;
            if ((imm = parse_number(const_val)) != -1) {
                if (imm < target->constants->imm_limit){
                    const_values[const_count] = imm;
                    if (verbose) printf("Preprocessor: Constant '%s' defined as %d\n", constants[const_count], const_values[const_count]); 
                } else {
                    return compile_error("Constants must be between 1 and %d", target->constants->imm_limit - 1);
                }
            }
        } else if (smatch(line, "LABEL")) {
            stmirror("LABEL\n", statements);
        }else {
            int index=0;
            if (ssplit(line, NULL, &index, "=")){
                stmirror("=", statements);
            }
            for(int i = 0; i < target->constants->opcount; i++) {
                if (ssplit(line, NULL, &index, target->oplist[0][i])){
                    stmirror(target->oplist[0][i], statements);
                }
            }
            if (ssplit(line, NULL, &index, ";")){
                stmirror(";", statements);
            }
            if (index == 0) {
                stmirror("J", statements);
            }
            stmirror("\n", statements);
        }

        
    }
    char stateline[linesize];
    rewind(input);
    if(mirror) rewind(state_mirror);
    rewind(statements);
    if(verbose) printf("Expanding macros\n");
    if(verbose) printf("Replacing constants and labels\n");
    // Replacing constants
    while (1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        } 
        linenum++;
        char line[linesize];
        fgets(stateline, linesize, statements);
        sclean(rawline, line);
        if (!smatch(stateline, "blank")){ 

            if (smatch(stateline, "LABEL")) { 
                if(verbose) printf("Preprocessor: label found\n");
                prmirror(line, processed_input);
                prmirror("\n", processed_input);
                continue;
            } else {
                int index = 0;
                for(int cha = 0; cha < strlen(stateline); cha++) {
                    char proc_statement[linesize] = "";
                    char fproc_statement[linesize] = "";
                    char char_string[2] = {'\0', '\0'};
                    char_string[0] = stateline[cha];
                    if(verbose) printf("Preprocessor: checking for statement %s\n", char_string[0] == '\n' ? "newline" : char_string);
                    int const_found = 0;
                    ssplit(line, proc_statement, &index, char_string);
                    if(verbose) printf("Preprocessor: statement found %s\n", proc_statement);
                    for (int cons = 0; cons <= const_count; cons ++) {
                        if(strcmp(proc_statement, constants[cons]) == 0) {
                            if(verbose) printf("%s is const: %s with value %d\n", proc_statement, constants[cons], const_values[cons]);
                            sprintf(fproc_statement, "%d", const_values[cons]);
                            if(verbose) printf("Preprocessor: outputting statement %s\n", fproc_statement);
                            prmirror(fproc_statement, processed_input);
                            const_found = 1;
                        }
                    }
                    if (!const_found){
                        sprintf(fproc_statement, "%s", proc_statement);
                        if(verbose) printf("Preprocessor: no constant found, outputting statement %s\n", fproc_statement);
                        prmirror(fproc_statement, processed_input);
                    }
                    if (char_string[0] == '\n'){
                        fprintf(processed_input, " %d", linenum);
                        if(mirror) fprintf(proc_mirror, " %d", linenum);
                    }
                    prmirror(char_string, processed_input);
                }
                if(verbose) printf("\n");
            }
        }
    }
    if (const_count != -1) {
        for (int i = 0; i <= const_count; i++) {
            free(constants[i]);
        }
        free(constants);
        free(const_values);
    }
    if(mirror) {
        fclose(state_mirror);
        fclose(proc_mirror);
    }
    rewind(processed_input);
    rewind(statements);
    return 0;
}

/*
    Goal: take a provided line and output every delimiter separating statements IN ORDER 

    Example flow:
    in: statement0, out: statement1 = statement2
    in: statement1, out: value | in: statement2, out: statement3 + statement4
    in: statement3, out: value | in: statement4, out: statement5; statement6
    in: statement5, out: value | in: statement6, out: value
    final output: =+;

    Complex example flow: (needed for higher level language, good to support preemptively)
    in: statement0, out: statement1 = statement2
    in: statement1, out: value | in: statement2, out: statement3 - statement4
    in: statement3, out: statement5 & statement6 | in: statement4, out: (statement7)
    in: statement5, out: value | in: statement6, out: value | in: statement7, out: statement8 + statement9
    in: statement8, out: value | in: statement9, out: value
    final output: =&-(+)
*/
int find_statement(const char* line, char* out, struct comp_target* target) {
    int index=0;
    char first[linesize] = "";
    char second[linesize] = "";
    if (ssplit(line, first, &index, "=")){
        find_statement(first, out, target);
        out[index] = '=';
        find_statement(second, out, target);
        return 0;
    }
    for(int i = 0; i < target->constants->opcount; i++) {
        if (ssplit(line, first, &index, target->oplist[0][i])){
            
        }
    }
    if (ssplit(line, first, &index, ";")){
       
    }
    if (index == 0) {
       
    }
    
}