#include "preprocessor.hpp"
#include "preprocessor_utils.hpp"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <libgen.h>  // for dirname
#include <string.h>

#include "../utilities/computils.hpp"

void build_relative_path(const char *input_file, const char *relative_file, char *out, size_t out_size) {
    char path_copy[1024];
    strncpy(path_copy, input_file, sizeof(path_copy));
    path_copy[sizeof(path_copy)-1] = '\0';

    char *dir = dirname(path_copy);
    snprintf(out, out_size, "%s/%s", dir, relative_file);
}

int preprocess(const char* path) {
    f_string rawline(linesize);
    FILE* state_mirror; 
    FILE* proc_mirror; 
    if(preflect) {
        state_mirror = fopen("smirror.txt", "w+");
        proc_mirror = fopen("procmirror.txt", "w+");
    }
    if(pverbose) std::print("Preprocessor start\n");
    rewind(input);

    if(pverbose) std::print("Expanding macros\n");
    array_vector<FILE*> included;
    array_vector<macro_def> macros;
    array_vector<f_string> constants;
    array_vector<int> const_values;
    // finding macros
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        if (rawline.smatch("INCLUDE ")) {

            f_string newfile = rawline.slice("INCLUDE ").sclean();
            f_string newfile_relative;
            build_relative_path(path, newfile, newfile_relative, 1024);
            included.push_back(fopen(newfile_relative, "r"));
            if (!included.top()) {
                return compile_error("failed to open file '{}'", newfile);
            }
            while(1) {
                if(fgets(rawline, linesize, included.top()) == NULL) {
                    break;
                }
                if (rawline.smatch("START_MACRO ")) {


                    // grabs macro name
                    int macro_scan_index = 0;
                    rawline.slice("START_MACRO ").ssplit(macros.expand().macro_name, macro_scan_index, " ");

                    // grabs arg_count
                    f_string macro_buffer;
                    rawline.slice("START_MACRO ").ssplit(macro_buffer, macro_scan_index, " ");
                    macros().arg_count = atoi(macro_buffer);

                    // grabs arg names
                    macros().arg_names = (char**) malloc(sizeof(char*)*macros().arg_count);
                    f_string arg_name;
                    for (int macro_arg_loop = 0; macro_arg_loop < macros().arg_count; macro_arg_loop++) {
                        macros().arg_names[macro_arg_loop] = (char*) calloc(rawline.length(), sizeof(char));
                        rawline.slice("START_MACRO ").ssplit(arg_name, macro_scan_index, " ");
                        sclean(arg_name, macros.top().arg_names[macro_arg_loop]);
                    }

                    int line_count = -1;
                    while(1) {
                        if(fgets(rawline, linesize, included.top()) == NULL) {
                            return compile_error("no END_MACRO call found for {}", macros.top().macro_name);
                        }
                        if(rawline.smatch("END_MACRO ") && rawline.slice("END_MACRO ").smatch(macros.top().macro_name)) {
                            break;
                        }
                        line_count++;
                        if (line_count == 0) {
                            macros().body = (char**) malloc(sizeof(char*));
                        } else {
                            macros().body = (char**) realloc(macros().body, sizeof(char*)*(line_count+1));
                        }
                        macros().body[line_count] = (char*) calloc(strlen(rawline)+5, sizeof(char));
                        strcpy(macros().body[line_count], rawline);
                    }
                    macros().line_count = line_count + 1;

                    if(pverbose) std::print("macro name: {}, arg_count: {}, line_count: {}\n", macros().macro_name, macros().arg_count, macros().line_count);
                    if(pverbose) std::print("macro count: {}\n\n", macros.get_count());
                    
                } else if (rawline.smatch("DEFINE ")) {
                    int const_index = strlen("DEFINE ");

                    rawline.ssplit(constants.expand(), const_index, " ");

                    // extracts the constant value as a string into const_val
                    f_string const_val = rawline.slice(const_index).sclean();
        
                    // parses the number in const_val and either puts the number into const_values or throws an error
                    int imm;
                    if ((imm = parse_number(const_val)) != -1) {
                        if (imm < target->constants->imm_limit){
                            const_values += imm;
                            if (pverbose) std::print("Preprocessor: Constant '{}' defined as {}\n", constants(), const_values()); 
                        } else {
                            return compile_error("Constants must be between 1 and {}", target->constants->imm_limit - 1);
                        }
                    } else {
                        return compile_error("Immediate value was not a valid number");
                    }
                } else if (rawline.smatch("START_STATIC ")) {
                    /*
                    new goal: support compile-time evaluation of predefined expressions
                    these will be started with START_STATIC <name>: <type> <arg_count> (<arg_name>: <type>) ...
                    supported types will initially only be numbers, later maybe registers as well
                    these can be invoked using the $ operator, which therefore needs to be blacklisted
                     from the start of constant names, macro names, and labels
                    the main objective for now is to allow for a predefined expression like $upper(0xFFFF) to automatically convert to 0xF,
                     which can then be used in an operation. 
                    This would be handled after all other preprocessing, and they cannot alter the number of lines,
                    */
                }
            }
        }

    }
    rewind(input);


    FILE* expanded_input = tmpfile();
    FILE* macr_mirror; 
    if (preflect) {
        macr_mirror = fopen("macro_processing.txt", "w+");
    }
    int local_linenum = 0;
    // replacing macros
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        f_string cleanline = rawline.sclean();
        int macro_found = 0;
        for (int macro = 0; macro <= macros.get_count() && !macro_found; macro++) {
            if (cleanline.smatch( macros[macro].macro_name)) {
                if (macro_replace(macros, macro, rawline, local_linenum, expanded_input, macr_mirror, 0, NULL)) return 1;
                macro_found = 1;
            }
        }
        if (!macro_found) {

            //fputs(rawline, expanded_input);
            fputs(rawline, expanded_input); if(preflect) fputs(rawline, macr_mirror);
        }
        local_linenum++;
    }
    fclose(input);
    if (mirror) {
        fclose(macr_mirror);
    }
    input = expanded_input;
    rewind(input);
    

    for (auto& macro : macros) {
        for (int j = 0; j < macro.arg_count; j++) {
            free(macro.arg_names[j]);
        }
        free(macro.arg_names);
        for (int j = 0; j < macro.line_count; j++) {
            free(macro.body[j]);
        }
    }


    // statement processing
    
    int plinenum = 0;
    if(pverbose) printf("Finding statements, constants, and labels...\n");
    while(1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        }
        f_string line = rawline.sclean();
        f_string label_name;    
        
        if (line.length() == 0 || line[0] == '#' || rawline.smatch("INCLUDE ")) {
            stmirror("blank\n", statements);
        } else if (rawline.smatch("DEFINE ")) {
            stmirror("blank\n", statements);
            int const_index = strlen("DEFINE ");
            f_string new_const;
            rawline.ssplit(new_const, const_index, " ");
            for (auto t_const : constants) {
                if (new_const == t_const) {
                    return compile_error("Multiple constants with the same name are not allowed: '{}' at line {}", new_const, plinenum);
                }
            }
            constants.push_back(new_const);
            // extracts the constant value as a string into const_val
            auto const_val = rawline.slice(const_index).sclean();

            // parses the number in const_val and either puts the number into const_values or throws an error
            int imm;
            if ((imm = parse_number(const_val)) != -1) {
                if (imm < target->constants->imm_limit){
                    const_values += imm;
                    if (pverbose) std::print("Preprocessor: Constant '{}' defined as {}\n", constants(), const_values()); 
                } else {
                    return compile_error("Constants must be between 1 and {}", target->constants->imm_limit - 1);
                }
            } else {
                return compile_error("Immediate value was not a valid number");
            }
        } else if (has_label(rawline, label_name) != 0){
            stmirror("LABEL\n", statements);

            f_string label = label_name;
            
            for (auto constant : constants) {
                if (label_name == constant) {
                    return compile_error("Multiple labels with the same name are not allowed: '{}' at line {}", label, plinenum);
                }
            }
            constants.push_back(label_name);
            // confirms the label is in a valid line number to be stored as a constant, and adds it to the list of constants
            if (plinenum < target->constants->imm_limit){
                const_values += plinenum;
                if (pverbose) std::print("Preprocessor: Label '{}' defined as linenum {}\n", constants(), const_values()); 
            } else {
                return compile_error("Label addresses must be between 1 and {}", target->constants->imm_limit - 1);
            }
            
        } else {
            int index=0;
            if (line.ssplit(NULL, index, "=")){
                stmirror("=", statements);
            }
            for(int i = 0; i < target->constants->opcount; i++) {
                if (line.ssplit(NULL, index, target->oplist[0][i])){
                    stmirror(target->oplist[0][i], statements);
                }
            }
            if (line.ssplit(NULL, index, ";")){
                stmirror(";", statements);
            }
            if (index == 0 && line[0] == 'J') {
                stmirror("J", statements);
            }
            stmirror("\n", statements);
            plinenum++;
        }
    }
    f_string stateline;
    rewind(input);
    if(mirror) rewind(state_mirror);
    rewind(statements);
    

    if(pverbose) printf("Replacing constants and labels\n");
    plinenum = 0;
    // Replacing constants
    while (1) {
        if(fgets(rawline, linesize, input) == NULL) {
            break;
        } 
        plinenum++;
        f_string line = rawline.sclean();
        fgets(stateline, linesize, statements);
        if (!stateline.smatch("blank")){ 

            if (stateline.smatch("LABEL")) { 
                if(pverbose) printf("Preprocessor: label found\n");
                continue;
            } else {
                int index = 0;
                for(auto cha : stateline) {
                    f_string proc_statement(linesize);
                    f_string fproc_statement(linesize);
                    char char_string[2] = {'\0', '\0'};
                    char_string[0] = cha;
                    
                    int const_found = 0;
                    line.ssplit(proc_statement, index, char_string);
        
                    for (int cons = 0; cons <= constants.size(); cons ++) {
                        if(proc_statement == constants[cons]) {
                            if(pverbose) std::print("{} is const with value {}\n", proc_statement, const_values[cons]);
                            snprintf(fproc_statement, fproc_statement.length(), "%d", const_values[cons]);
                            
                            prmirror(fproc_statement, processed_input);
                            const_found = 1;
                        }
                    }
                    if (!const_found){
                        fproc_statement = proc_statement;
                        
                        prmirror(fproc_statement, processed_input);
                    }
                    if (char_string[0] == '\n'){
                        fprintf(processed_input, " %d", plinenum);
                        if(reflect) fprintf(proc_mirror, " %d", plinenum);
                    }
                    prmirror(char_string, processed_input);
                }
                //if(verbose) printf("\n");
            }
        }
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
int macro_replace(array_vector<macro_def>& macro_list, int macro_index, char* codeline, int linenum, FILE* output, FILE* macr_mirror, int layer, char* prefix) {
    if (layer > recursion_limit) {
        return compile_error("Excessive recursion in macros, final macro in recursion {}", curr_macro.macro_name);
    }
    if (layer == 0) {
        if(pverbose) std::println("starting macro '{}'", curr_macro.macro_name);
    } else {
        if(pverbose) std::println("inner macro: '{}'\n", curr_macro.macro_name);
    }

    f_string localprefix;
    if (prefix == NULL){
        localprefix = curr_macro.macro_name;
    } else {
        localprefix = f_string(prefix) + "_" + curr_macro.macro_name;
    }

    f_string cleanline = f_string(codeline).sclean_i('\n', 0);
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
        if(pverbose) std::println("arg '{}' has value '{}'", curr_macro.arg_names[arg_loop_index], const_values[arg_loop_index]);
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
            if(pverbose) printf("New constant '%s' with value '%s'\n", constants[const_index], const_values[const_index]);
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
                f_string labeltag = std::format("{}_{}_{}_", layer, linenum, localprefix).c_str();
                labeltag += filter_line;
                strcpy(const_values[const_index], labeltag);
                if(pverbose) std::print("New label '{}' with value '{}'\n", constants[const_index], const_values[const_index]);
            }
        }
    }
    FILE* localfile = tmpfile();
    // stores the current line, with constant values replaced
    f_string filled_line;
    // second, go through the file again and replace all constants and labels, and expand all macros
    for (int macro_line = 0; macro_line < curr_macro.line_count; macro_line++) {
        if (curr_macro_line[0] == '#'){
            //printf("%s\n", curr_macro_line);
            continue;
        }
        filled_line = "";

        // replace all constants and labels with assigned values
        f_string curr_segment;
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
                if(curr_segment == constants[cons]) {
                    curr_segment = const_values[cons];
                    if(pverbose) std::println("constant '{}' with value '{}' used", constants[cons], const_values[cons]);
                    const_found = 1;
                }
            }
            filled_line += curr_segment;
            if(label) filled_line += ':';
            if (end == 0) {
                filled_line += "\n";
                //printf("%d %s", macro_line, filled_line);
                break;
            }
            filled_line += " ";
        
        }
        //printf("%s", filled_line);
        
        if (!filled_line.smatch("DEFINE ")) {
            int macro_found = 0;
            for (int macro = 0; macro <= macro_list.get_count() && !macro_found; macro++) {
                if (smatch(filled_line, macro_list[macro].macro_name) && (filled_line[strlen(macro_list[macro].macro_name)] == ' ' || filled_line[strlen(macro_list[macro].macro_name)] == '\n')) {
                    if (macro_replace(macro_list, macro, filled_line, macro_line, localfile, macr_mirror, layer+1, localprefix) == 1) return 1;
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
    f_string rawline = "# ";
    rawline += cleanline + '\n';
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
    if (pverbose) printf("Preprocessor: macro '%s' expanded", macro_list[macro_index].macro_name);
    if (layer == 0 && pverbose) printf("--\n");
    if(pverbose) printf("\n");
    return 0;
}
