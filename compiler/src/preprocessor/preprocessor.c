#include "preprocessor.h"
#include "preprocessor_utils.h"
#include <stdlib.h>
#include <string.h>

#include "../utilities/computils.h"



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
                        macros[macro_count].body[line_count] = (char*) calloc(strlen(rawline)+5, sizeof(char));
                        strcpy(macros[macro_count].body[line_count], rawline);
                    }
                    macros[macro_count].line_count = line_count + 1;

                    if(verbose) printf("macro name: %s, arg_count: %d, line_count: %d\n", macros[macro_count].macro_name, macros[macro_count].arg_count, macros[macro_count].line_count);
                    if(verbose) printf("macro count: %d\n\n", macro_count);
                    
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
    int local_linenum = 0;
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
                if (macro_replace(macros, macro_count, macro, rawline, local_linenum, expanded_input, mirror, macr_mirror, verbose, 0, NULL)) return 1;
                macro_found = 1;
            }
        }
        if (!macro_found) {
            //fputs(rawline, expanded_input);
            exmirror(rawline, expanded_input);
        }
        local_linenum++;
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
            for (int j = 0; j < macros[i].line_count; j++) {
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
        char label_name[linesize];    
        
        if (strlen(line) == 0 || line[0] == '#' || smatch(rawline, "INCLUDE ")) {
            stmirror("blank\n", statements);
        } else if (smatch(rawline, "DEFINE ")) {
            stmirror("blank\n", statements);
            int const_index = strlen("DEFINE ");
        
            expand_constants(const_count, constants, const_values);
            ssplit(rawline, constants[const_count], &const_index, " ");
            for (int test = 0; test < const_count; test++) {
                if (strcmp(constants[const_count], constants[test]) == 0) {
                    return compile_error("Multiple constants with the same name are not allowed: '%s' at line %d", constants[const_count], plinenum);
                }
            }
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
        } else if (has_label(rawline, label_name) != 0){
            stmirror("LABEL\n", statements);
            expand_constants(const_count, constants, const_values);
            strcpy(constants[const_count], label_name);
            for (int test = 0; test < const_count; test++) {
                if (strcmp(constants[const_count], constants[test]) == 0) {
                    return compile_error("Multiple labels with the same name are not allowed: '%s' at line %d", constants[const_count], plinenum);
                }
            }
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
                    //if(verbose) printf("Preprocessor: checking for statement %s\n", char_string[0] == '\n' ? "newline" : char_string);
                    int const_found = 0;
                    ssplit(line, proc_statement, &index, char_string);
                    //if(verbose) printf("Preprocessor: statement found %s\n", proc_statement);
                    for (int cons = 0; cons <= const_count; cons ++) {
                        if(strcmp(proc_statement, constants[cons]) == 0) {
                            if(verbose) printf("%s is const with value %d\n", proc_statement, const_values[cons]);
                            sprintf(fproc_statement, "%d", const_values[cons]);
                            //if(verbose) printf("Preprocessor: outputting statement %s\n", fproc_statement);
                            prmirror(fproc_statement, processed_input);
                            const_found = 1;
                        }
                    }
                    if (!const_found){
                        sprintf(fproc_statement, "%s", proc_statement);
                        //if(verbose) printf("Preprocessor: no constant found, outputting statement %s\n", fproc_statement);
                        prmirror(fproc_statement, processed_input);
                    }
                    if (char_string[0] == '\n'){
                        fprintf(processed_input, " %d", plinenum);
                        if(mirror) fprintf(proc_mirror, " %d", plinenum);
                    }
                    prmirror(char_string, processed_input);
                }
                //if(verbose) printf("\n");
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
int macro_replace(macro_def* macro_list, int macro_count, int macro_index, char* codeline, int linenum, FILE* output, int mirror, FILE* macr_mirror, int verbose, int layer, char* prefix) {
    if (layer > recursion_limit) {
        return compile_error("Excessive recursion in macros, final macro in recursion %s", curr_macro.macro_name);
    }
    if (layer == 0) {
        if(verbose) printf("starting macro '%s'\n", curr_macro.macro_name);
    } else {
        if(verbose) printf("inner macro: '%s'\n", curr_macro.macro_name);
    }

    char localprefix[linesize];
    if (prefix == NULL){
        strcpy(localprefix, curr_macro.macro_name);
    } else {
        sprintf(localprefix, "%s_%s", prefix, curr_macro.macro_name);
    }

    char cleanline[linesize] = "";
    sclean_i(codeline, cleanline, '\n', 0);
    // grab the provided arg values
    int const_index = curr_macro.arg_count - 1;
    char** constants;
    char** const_values;
    if (const_index >= 0) {
        constants = (char**) malloc(sizeof(char*)*(const_index+1));
        const_values = (char**) malloc(sizeof(char*)*(const_index+1));
    }
    int arg_scan_index = strlen(curr_macro.macro_name) + 1;

    for(int arg_loop_index = 0; arg_loop_index <= const_index; arg_loop_index++) {
        // allocate memory and add the argument name as a constant
        constants[arg_loop_index] = (char*) calloc(linesize, sizeof(char));
        const_values[arg_loop_index] = (char*) calloc(linesize, sizeof(char));
        strcpy(constants[arg_loop_index], curr_macro.arg_names[arg_loop_index]);

        if (ssplit(cleanline, const_values[arg_loop_index], &arg_scan_index, " ") == 0) {
            sclean(s_slice(cleanline, arg_scan_index), const_values[arg_loop_index]);
        }
        if(verbose) printf("arg '%s' has value '%s'\n", curr_macro.arg_names[arg_loop_index], const_values[arg_loop_index]);
    }

    // first, filter through the file to define all constants as well as rename inner labels (via constants)
    //FILE* filterfile = tmpfile();
    char filter_line[linesize];
    for (int macro_line = 0; macro_line < curr_macro.line_count; macro_line++) {
        strcpy(filter_line, "");
        // next, parse the line for macros, definitions, or labels
        if (smatch(curr_macro_line, "DEFINE ")) {
            int def_index = strlen("DEFINE ");
            const_index++;
            if (const_index == 0) {
                constants = (char**) malloc(sizeof(char*));
                const_values = (char**) malloc(sizeof(char*));
            } else {
                constants = (char**) realloc(constants, sizeof(char*)*(const_index+1));
                const_values = (char**) realloc(const_values, sizeof(char*)*(const_index+1));
            }
            constants[const_index] = (char*) calloc(linesize, sizeof(char));
            const_values[const_index] = (char*) calloc(linesize, sizeof(char));
            ssplit(curr_macro_line, constants[const_index], &def_index, " ");

            // extracts the constant value as a string into const_val
            sclean(s_slice(curr_macro_line, def_index), const_values[const_index]);
            if(verbose) printf("New constant '%s' with value '%s'\n", constants[const_index], const_values[const_index]);
        } else if (has_label(curr_macro_line, filter_line) == 1) {
            int not_arg = 1;
            for (int arg_check = 0; arg_check < curr_macro.arg_count && not_arg; arg_check++){
                if (strcmp(filter_line, constants[arg_check]) == 0) {
                    not_arg = 0;
                }
            }

            if (not_arg) {
                const_index++;
                if (const_index == 0) {
                    constants = (char**) malloc(sizeof(char*));
                    const_values = (char**) malloc(sizeof(char*));
                } else {
                    constants = (char**) realloc(constants, sizeof(char*)*(const_index+1));
                    const_values = (char**) realloc(const_values, sizeof(char*)*(const_index+1));
                }
                constants[const_index] = (char*) calloc(linesize, sizeof(char));
                const_values[const_index] = (char*) calloc(linesize, sizeof(char));

                strcpy(constants[const_index], filter_line);

                // creates unique label tag
                char labeltag[linesize] = "";
                sprintf(labeltag, "%d_%d_%s_", layer, linenum, localprefix);
                strcat(labeltag, filter_line);
                strcpy(const_values[const_index], labeltag);
                if(verbose) printf("New label '%s' with value '%s'\n", constants[const_index], const_values[const_index]);
            }
        }
    }
    FILE* localfile = tmpfile();
    // stores the current line, with constant values replaced
    char filled_line[linesize];
    // second, go through the file again and replace all constants and labels, and expand all macros
    for (int macro_line = 0; macro_line < curr_macro.line_count; macro_line++) {
        if (curr_macro_line[0] == '#'){
            //printf("%s\n", curr_macro_line);
            continue;
        }
        strcpy(filled_line, "");

        // replace all constants and labels with assigned values
        char curr_segment[linesize];
        int segment_index = 0;
        while(1){
            int const_found = 0;
            int label = 0;
            char raw_segment[linesize];
            // find the next chunk of space-separated characters, if ssplit fails then the rest of the line is the last chunk
            int end = ssplit(curr_macro_line, raw_segment, &segment_index, " ");
            if (end == 0) sclean(s_slice(curr_macro_line, segment_index), raw_segment);
            sclean_i(raw_segment, curr_segment, ':', 0);

           
            if (strcmp(raw_segment, curr_segment) != 0) {
                label = 1;
            }
            for (int cons = 0; cons <= const_index && !const_found; cons ++) {
                if(strcmp(curr_segment, constants[cons]) == 0) {
                    sprintf(curr_segment, "%s", const_values[cons]);
                    if(verbose) printf("constant '%s' with value '%s' used\n", constants[cons], const_values[cons]);
                    const_found = 1;
                }
            }
            strcat(filled_line, curr_segment);
            if(label) strcat(filled_line, ":");
            if (end == 0) {
                strcat(filled_line, "\n");
                //printf("%d %s", macro_line, filled_line);
                break;
            }
            strcat(filled_line, " ");
        
        }
        //printf("%s", filled_line);
        
        if (!smatch(filled_line, "DEFINE ")) {
            int macro_found = 0;
            for (int macro = 0; macro <= macro_count && !macro_found; macro++) {
                if (smatch(filled_line, macro_list[macro].macro_name) && (filled_line[strlen(macro_list[macro].macro_name)] == ' ' || filled_line[strlen(macro_list[macro].macro_name)] == '\n')) {
                    if (macro_replace(macro_list, macro_count, macro, filled_line, macro_line, localfile, mirror, macr_mirror, verbose, layer+1, localprefix) == 1) return 1;
                    macro_found = 1;
                }
            }
            if (!macro_found) {
                fputs(filled_line, localfile);
            }
        }
    }
    if (const_index > -1) {
        for (int i = 0; i <= const_index; i++) {
            free(constants[i]);
            free(const_values[i]);
        }
        free(constants);
        free(const_values);
    }
    rewind(localfile);
    char rawline[linesize];
    sprintf(rawline, "# %s\n", cleanline);
    if (layer == 0) {
        exmirror("# TOP MACRO \n", output);
        exmirror(rawline, output);
    } else {
        fputs(rawline, output);
    }
    while(1) {
        if(fgets(rawline, linesize, localfile) == NULL) {
            break;
        }
        //fputs(rawline, output);
        if (layer == 0) {
            exmirror(rawline, output);
        } else {
            fputs(rawline, output);
        }
        
        //printf("%s", rawline);
    }
    if (layer == 0) {
        exmirror("\n", output);
    }
    
    fclose(localfile);
    if (verbose) printf("Preprocessor: macro '%s' expanded", macro_list[macro_index].macro_name);
    if (layer == 0 && verbose) printf("--\n");
    if(verbose) printf("\n");
    return 0;
}
