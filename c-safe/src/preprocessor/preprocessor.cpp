#include "preprocessor.hpp"
#include "preprocessor_utils.hpp"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <libgen.h>  // for dirname
#include <string.h>

#include "../utilities/computils.hpp"


int preprocess(const char* path) {
    f_string rawline;
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
    array_vector<static_def> statics;
    array_vector<f_string> constants;
    array_vector<int> const_values;
    // finding macros
    if (macro_scan(macros, constants, const_values, statics, input, path) == -1) return -1;


    FILE* expanded_input = tmpfile();
    FILE* macr_mirror; 
    if (preflect) {
        macr_mirror = fopen("macro_processing.txt", "w+");
    }
    int local_linenum = 0;
    bool in_def = false;
    // replacing macros
    while(1) {
        if(rawline.fgets(linesize, input) == NULL) {
            break;
        }
        f_string cleanline = rawline.sclean();
        int replacement_found = 0;
        for (int macro = 0; macro <= macros.get_count() && !replacement_found; macro++) {
            if (cleanline.smatch( macros[macro].macro_name)) {
                if (macro_replace(macros, macro, rawline, local_linenum, expanded_input, macr_mirror, 0, NULL)) return 1;
                replacement_found = 1;
            }
        }
        if (!replacement_found) {
            if (rawline.smatch("START_MACRO ") || rawline.smatch("START_STATIC ")) {
                in_def = true;
            } else if (in_def) {
                if (rawline.smatch("END_MACRO ") || rawline.smatch("END_STATIC ")) {
                    in_def = false;
                }
            } else {
                //fputs(rawline, expanded_input);
                fputs(rawline, expanded_input); if(preflect) fputs(rawline, macr_mirror);
            }
        }
        local_linenum++;
    }
    fclose(input);
    if (mirror) {
        fclose(macr_mirror);
    }
    input = expanded_input;
    rewind(input);


    // statement processing
    
    int plinenum = 0;
    if(pverbose) printf("Finding statements, constants, and labels...\n");
    while(1) {
        if(rawline.fgets(linesize, input) == NULL) {
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
            rawline.ssplit(new_const, &const_index, " ");
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
            constants.push_back(std::move(label_name));
            // confirms the label is in a valid line number to be stored as a constant, and adds it to the list of constants
            if (plinenum < target->constants->imm_limit){
                const_values += plinenum;
                if (pverbose) std::print("Preprocessor: Label '{}' defined as linenum {}\n", constants(), const_values()); 
            } else {
                return compile_error("Label addresses must be between 1 and {}", target->constants->imm_limit - 1);
            }
            
        } else {
            int index=0;
            if (line.ssplit(NULL, &index, "=")){
                stmirror("=", statements);
            }
            for(int i = 0; i < target->constants->opcount; i++) {
                if (line.ssplit(NULL, &index, target->oplist[0][i])){
                    stmirror(target->oplist[0][i], statements);
                }
            }
            if (line.ssplit(NULL, &index, ";")){
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
        if(rawline.fgets(linesize, input) == NULL) {
            break;
        } 
        plinenum++;
        f_string line = rawline.sclean();
        stateline.fgets(linesize, statements);
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
                    line.ssplit(proc_statement, &index, char_string);
        
                    for (int cons = 0; cons < constants.size(); cons ++) {
                        if(proc_statement == constants[cons]) {
                            if(pverbose) std::print("{} is const with value {}\n", proc_statement, const_values[cons]);
                            fproc_statement = std::format("{}", const_values[cons]).c_str();
                            
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


/**
 * Takes the list of macros, the current macro to insert, and outputs the fully expanded macro to the provided output file
 */
int macro_replace(array_vector<macro_def>& macro_list, int macro_index, f_string codeline, int linenum, FILE* output, FILE* macr_mirror, int layer, char* prefix) {
    const macro_def& curr_macro = macro_list[macro_index];

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

    f_string cleanline = codeline.sclean_i('\n', 0);
    // grab the provided arg values
    array_vector<f_string> constants;
    array_vector<f_string> const_values;
    int arg_scan_index = strlen(curr_macro.macro_name) + 1;

    for(int arg_loop_index = 0; arg_loop_index <= (curr_macro.arg_count - 1); arg_loop_index++) {
        // allocate memory and add the argument name as a constant
        constants += curr_macro.arg_names[arg_loop_index];
        const_values.expand();

        if (cleanline.ssplit(const_values[arg_loop_index], &arg_scan_index, " ") == 0) {
            const_values[arg_loop_index] = const_values[arg_loop_index].sclean();
        }
        if(pverbose) std::println("arg '{}' has value '{}'", curr_macro.arg_names[arg_loop_index], const_values[arg_loop_index]);
    }

    // first, filter through the file to define all constants as well as rename inner labels (via constants)
    //FILE* filterfile = tmpfile();
    f_string filter_line;
    for (int macro_line = 0; macro_line < curr_macro.line_count; macro_line++) {
        f_string curr_macro_line = macro_list[macro_index].body[macro_line];
        filter_line = "";
        // next, parse the line for macros, definitions, or labels
        if (curr_macro_line.smatch("DEFINE ")) {
            int def_index = strlen("DEFINE ");
            curr_macro_line.ssplit(constants.expand(), &def_index, " ");

            // extracts the constant value as a string into const_val
            const_values += curr_macro_line.slice(def_index).sclean();
            if(pverbose) std::println("New constant '{}' with value '{}'\n", constants(), const_values());
        } else if (has_label(curr_macro_line, filter_line) == 1) {
            int not_arg = 1;
            for (int arg_check = 0; arg_check < curr_macro.arg_count && not_arg; arg_check++){
                if (filter_line == constants[arg_check]) {
                    not_arg = 0;
                }
            }

            if (not_arg) {
                constants += filter_line;

                // creates unique label tag
                f_string labeltag = std::format("{}_{}_{}_", layer, linenum, localprefix).c_str();
                labeltag += filter_line;
                const_values += labeltag;
                if(pverbose) std::println("New label '{}' with value '{}'", constants(), const_values());
            }
        }
    }
    FILE* localfile = tmpfile();
    // stores the current line, with constant values replaced
    f_string filled_line;
    // second, go through the file again and replace all constants and labels, and expand all macros
    for (int macro_line = 0; macro_line < curr_macro.line_count; macro_line++) {
        const f_string curr_macro_line = macro_list[macro_index].body[macro_line];
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
            f_string raw_segment;
            // find the next chunk of space-separated characters, if ssplit fails then the rest of the line is the last chunk
            int end = curr_macro_line.ssplit(raw_segment, &segment_index, " ");
            if (end == 0) raw_segment = curr_macro_line.slice(segment_index).sclean();
            curr_segment = raw_segment.sclean_i(':', 0);

           
            if (raw_segment != curr_segment) {
                label = 1;
            }
            for (int cons = 0; cons <= constants.get_count() && !const_found; cons ++) {
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
                if (filled_line.smatch(macro_list[macro].macro_name) && (filled_line[strlen(macro_list[macro].macro_name)] == ' ' || filled_line[strlen(macro_list[macro].macro_name)] == '\n')) {
                    if (macro_replace(macro_list, macro, filled_line, macro_line, localfile, macr_mirror, layer+1, localprefix) == 1) return 1;
                    macro_found = 1;
                }
            }
            if (!macro_found) {
                fputs(filled_line, localfile);
            }
        }
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
        if(rawline.fgets(linesize, localfile) == NULL) {
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
    if (pverbose) std::print("Preprocessor: macro '{}' expanded", macro_list[macro_index].macro_name);
    if (layer == 0 && pverbose) printf("--\n");
    if(pverbose) printf("\n");
    return 0;
}
