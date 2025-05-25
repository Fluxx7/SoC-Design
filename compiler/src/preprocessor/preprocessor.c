#include "preprocessor.h"
#include <stdlib.h>
#include <string.h>

#include "../utilities/computils.h"
#define stmirror(string, file) fputs(string, file); if(mirror) fputs(string, state_mirror)
#define prmirror(string, file) fputs(string, file); if(mirror) fputs(string, proc_mirror)
#define exmirror(string, file) fputs(string, file); if(mirror) fputs(string, macr_mirror)
#define expand_constants(counter, name_list, val_list) \
counter++;\
if (counter == 0) {\
    name_list = (char**) malloc(sizeof(char*));\
    val_list = (int*) malloc(sizeof(int));\
} else {\
    name_list = (char**) realloc(name_list, sizeof(char*)*(counter+1));\
    val_list = (int*) realloc(val_list, sizeof(int)*(counter+1));\
} \
name_list[counter] = (char*) calloc(linesize, sizeof(char));

typedef struct macro_def {
    char macro_name[linesize];
    int arg_count;
    char** arg_names;
    char** body;
    int line_count;
} macro_def;

int macro_replace(macro_def*, int, int, char*, FILE*, int, FILE*, int, int);

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

    if(verbose) printf("Expanding macros\n");
    FILE** included;
    macro_def* macros;
    int macro_count = -1;
    int include_count = -1;
    char** constants;
    int* const_values;
    int const_count = -1;
    // finding macros
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        if (smatch(rawline, "INCLUDE ")) {
            // expands the list of included files by one
            include_count++;
            if (include_count == 0) {
                
                included = (FILE**) malloc(sizeof(FILE*));
            } else {
                included = (FILE**) realloc(included, sizeof(FILE*)*(include_count+1));
            } 

            char newfile[linesize];
            sclean(s_slice(rawline, strlen("INCLUDE ")), newfile);
            included[include_count] = fopen(newfile, "r");
            while(1) {
                if(fgets(rawline, linesize, included[include_count]) == NULL) {
                    break;
                }
                if (smatch(rawline, "START_MACRO ")) {
                    // expands the list of macros by one
                    macro_count++;
                    if (macro_count == 0) {
                        macros = (macro_def*) malloc(sizeof(macro_def));
                    } else {
                        macros = (macro_def*) realloc(macros, sizeof(macro_def)*(macro_count+1));
                    }

                    // grabs macro name
                    int macro_scan_index = 0;
                    ssplit(s_slice(rawline, strlen("START_MACRO ")), macros[macro_count].macro_name, &macro_scan_index, " ");

                    // grabs arg_count
                    char macro_buffer[linesize];
                    ssplit(s_slice(rawline, strlen("START_MACRO ")), macro_buffer, &macro_scan_index, " ");
                    macros[macro_count].arg_count = atoi(macro_buffer);

                    // grabs arg names
                    macros[macro_count].arg_names = (char**) malloc(sizeof(char*)*macros[macro_count].arg_count);
                    char arg_name[linesize];
                    for (int macro_arg_loop = 0; macro_arg_loop < macros[macro_count].arg_count; macro_arg_loop++) {
                        macros[macro_count].arg_names[macro_arg_loop] = (char*) calloc(strlen(rawline), sizeof(char));
                        ssplit(s_slice(rawline, strlen("START_MACRO ")), arg_name, &macro_scan_index, " ");
                        sclean(arg_name, macros[macro_count].arg_names[macro_arg_loop]);
                    }

                    int line_count = -1;
                    while(1) {
                        if(fgets(rawline, linesize, included[include_count]) == NULL) {
                            return compile_error("no END_MACRO call found for %s", macros[macro_count].macro_name);
                        }
                        if(smatch(rawline, "END_MACRO ") && smatch(s_slice(rawline, strlen("END_MACRO ")), macros[macro_count].macro_name)) {
                            break;
                        }
                        line_count++;
                        if (line_count == 0) {
                            macros[macro_count].body = (char**) malloc(sizeof(char*));
                        } else {
                            macros[macro_count].body = (char**) realloc(macros[macro_count].body, sizeof(char*)*(line_count+1));
                        }
                        macros[macro_count].body[line_count] = (char*) calloc(strlen(rawline), sizeof(char));
                        strcpy(macros[macro_count].body[line_count], rawline);
                    }
                    macros[macro_count].line_count = line_count;

                    printf("macro name: %s, arg_count: %d, line_count: %d\n", macros[macro_count].macro_name, macros[macro_count].arg_count, macros[macro_count].line_count);
                    printf("macro count: %d\n\n", macro_count);
                    
                } else if (smatch(rawline, "DEFINE ")) {
                    int const_index = strlen("DEFINE ");
                
                    expand_constants(const_count, constants, const_values);
                    ssplit(rawline, constants[const_count], &const_index, " ");
        
                    // extracts the constant value as a string into const_val
                    char const_val[strlen(s_slice(rawline, const_index))];
                    sclean(s_slice(rawline, const_index), const_val);
        
                    // parses the number in const_val and either puts the number into const_values or throws an error
                    int imm;
                    if ((imm = parse_number(const_val)) != -1) {
                        if (imm < target->constants->imm_limit){
                            const_values[const_count] = imm;
                            if (verbose) printf("Preprocessor: Constant '%s' defined as %d\n", constants[const_count], const_values[const_count]); 
                        } else {
                            return compile_error("Constants must be between 1 and %d", target->constants->imm_limit - 1);
                        }
                    } else {
                        return compile_error("Immediate value was not a valid number");
                    }
                }
            }
        }

    }
    rewind(input);
    FILE* expanded_input = tmpfile();
    FILE* macr_mirror; 
    if (mirror) {
        macr_mirror = fopen("macro_processing.txt", "w+");
    }
    // replacing macros
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        char cleanline[linesize];
        sclean(rawline, cleanline);
        int macro_found = 0;
        for (int macro = 0; macro <= macro_count && !macro_found; macro++) {
            if (smatch(cleanline, macros[macro].macro_name)) {
                // this needs to insert the macro while replacing any arguments it takes with the provided values
                // should probably also check to see if the macro contains any macros
                // therefore this should probably be a recursive function
                if (macro_replace(macros, macro_count, macro, rawline, expanded_input, mirror, macr_mirror, verbose, 0) == -1) return -1;
                exmirror("\n", expanded_input);
                macro_found = 1;
            }
        }
        if (!macro_found) {
            exmirror(rawline, expanded_input);
        }
    }
    fclose(input);
    if (mirror) {
        fclose(macr_mirror);
    }
    input = expanded_input;
    rewind(input);
    if (macro_count != -1) {
        for (int i = 0; i <= macro_count; i++) {
            for (int j = 0; j < macros[i].arg_count; j++) {
                free(macros[i].arg_names[j]);
            }
            free(macros[i].arg_names);
            for (int j = 0; j <= macros[i].line_count; j++) {
                free(macros[i].body[j]);
            }
        }
        free(macros);
    }
    if (include_count != -1) {
        for (int i = 0; i <= include_count; i++) {
            fclose(included[i]);
        }
        free(included);
    }
    /* 
    macro targeting: 
    START_MACRO <macro name> <number of arguments> <arg names>
    default arg names are arg<arg_num> ie arg0, arg1, arg2, arg names are set in numerical order
    <macro code>
    END_MACRO <macro name>

    to call a macro, just use <macro name>

    use INCLUDE <file address> to use a file defining existing macros and constants 
    this file WILL NOT be run through the compiler, ONLY through the preprocessor to grab definitions
    */

    // statement processing
    
    int plinenum = 0;
    if(verbose) printf("Finding statements, constants, and labels...\n");
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        char line[linesize];
        sclean(rawline, line);
        if (strlen(line) == 0 || line[0] == '#' || smatch(rawline, "INCLUDE ")) {
            stmirror("blank\n", statements);
        } else if (smatch(rawline, "DEFINE ")) {
            stmirror("blank\n", statements);
            int const_index = strlen("DEFINE ");
        
            expand_constants(const_count, constants, const_values);
            ssplit(rawline, constants[const_count], &const_index, " ");

            // extracts the constant value as a string into const_val
            char const_val[strlen(s_slice(rawline, const_index))];
            sclean(s_slice(rawline, const_index), const_val);

            // parses the number in const_val and either puts the number into const_values or throws an error
            int imm;
            if ((imm = parse_number(const_val)) != -1) {
                if (imm < target->constants->imm_limit){
                    const_values[const_count] = imm;
                    if (verbose) printf("Preprocessor: Constant '%s' defined as %d\n", constants[const_count], const_values[const_count]); 
                } else {
                    return compile_error("Constants must be between 1 and %d", target->constants->imm_limit - 1);
                }
            } else {
                return compile_error("Immediate value was not a valid number");
            }
        } else if (smatch(line, "LABEL")) {
            // labels are stored as a constant with the value being their line number
            stmirror("LABEL\n", statements);
            

            expand_constants(const_count, constants, const_values);

            // puts the label into constants[const_count]
            char labelname[linesize];
            sclean(s_slice(rawline, strlen("LABEL ")), labelname);
            sclean_i(labelname, constants[const_count], ':', 0);

            // confirms the label is in a valid line number to be stored as a constant, and adds it to the list of constants
            if (plinenum < target->constants->imm_limit){
                const_values[const_count] = plinenum;
                if (verbose) printf("Preprocessor: Label '%s' defined as linenum %d\n", constants[const_count], const_values[const_count]); 
            } else {
                return compile_error("Label addresses must be between 1 and %d", target->constants->imm_limit - 1);
            }
        
        } else if (line[strlen(line)-1] == ':') {
            // same as previous label code, handles no LABEL marker
            stmirror("LABEL\n", statements);
            

            expand_constants(const_count, constants, const_values);

            // puts the label into constants[const_count]
            char labelname[linesize];
            sclean(rawline, labelname);
            sclean_i(labelname, constants[const_count], ':', 0);

            // confirms the label is in a valid line number to be stored as a constant, and adds it to the list of constants
            if (plinenum < target->constants->imm_limit){
                const_values[const_count] = plinenum;
                if (verbose) printf("Preprocessor: Label '%s' defined as linenum %d\n", constants[const_count], const_values[const_count]); 
            } else {
                return compile_error("Label addresses must be between 1 and %d", target->constants->imm_limit - 1);
            }
        
        } else {
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
            if (index == 0 && line[0] == 'J') {
                stmirror("J", statements);
            }
            stmirror("\n", statements);
            plinenum++;
        }
    }
    char stateline[linesize];
    rewind(input);
    if(mirror) rewind(state_mirror);
    rewind(statements);
    

    if(verbose) printf("Replacing constants and labels\n");
    plinenum = 0;
    // Replacing constants
    while (1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        } 
        plinenum++;
        char line[linesize];
        fgets(stateline, linesize, statements);
        sclean(rawline, line);
        if (!smatch(stateline, "blank")){ 

            if (smatch(stateline, "LABEL")) { 
                if(verbose) printf("Preprocessor: label found\n");
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
                        fprintf(processed_input, " %d", plinenum);
                        if(mirror) fprintf(proc_mirror, " %d", plinenum);
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



#define curr_macro macro_list[macro_index]
#define curr_macro_line curr_macro.body[macro_line]
/**
 * Takes the list of macros, the current macro to insert, and outputs the fully expanded macro to the provided output file
 */
int macro_replace(macro_def* macro_list, int macro_count, int macro_index, char* codeline,  FILE* output, int mirror, FILE* macr_mirror, int verbose, int layer) {
    if (layer > recursion_limit) {
        return compile_error("Excessive recursion in macros, final macro in recursion %s", curr_macro.macro_name);
    }
    if (layer == 0) {
        printf("starting macro '%s'\n", curr_macro.macro_name);
    } else {
        printf("inner macro: '%s'\n", curr_macro.macro_name);
    }
    

    char cleanline[linesize];
    sclean_i(codeline, cleanline, '\n', 0);
    // grab the provided arg values
    char arg_values[curr_macro.arg_count][linesize];
    int arg_scan_index = strlen(curr_macro.macro_name) + 1;
    for(int arg_loop_index = 0; arg_loop_index < curr_macro.arg_count; arg_loop_index++) {
        if (ssplit(cleanline, arg_values[arg_loop_index], &arg_scan_index, " ") == 0) {
            sclean(s_slice(cleanline, arg_scan_index), arg_values[arg_loop_index]);
            //printf("here\n");
        }
        //printf("codeline: %s, index: %d\n", cleanline, arg_scan_index);
        printf("arg '%s' has value '%s'\n", curr_macro.arg_names[arg_loop_index], arg_values[arg_loop_index]);
    }
    
    FILE* localfile = tmpfile();

    for (int macro_line = 0; macro_line < curr_macro.line_count; macro_line++) {
        
        int macro_found = 0;
        for (int macro = 0; macro <= macro_count && !macro_found; macro++) {
            if (smatch(curr_macro_line, macro_list[macro].macro_name) && curr_macro_line[strlen(macro_list[macro].macro_name)] == ' ') {
                if (macro_replace(macro_list, macro_count, macro, curr_macro_line, localfile, mirror, macr_mirror, verbose, layer+1) == -1) return -1;
                
                macro_found = 1;
            }
        }
        if (!macro_found) {
            
        }
    }
    rewind(localfile);
    char rawline[linesize];
    while(1) {
        if(fgets(rawline, linesize, localfile) == NULL) {
            break;
        }
        exmirror(rawline, output);
    }
    fclose(localfile);
    if (verbose) printf("Preprocessor: macro '%s' expanded", macro_list[macro_index].macro_name);
    if (layer == 0) printf("\n");
    return 0;
}