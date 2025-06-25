#include "preprocessor.hpp"
#include "preprocessor_utils.hpp"
#include <stdlib.h>
#include <string.h>
#include <libgen.h>  // for dirname

#include "../utilities/computils.hpp"

// returns 0 if no label found, 1 if a label is successfully identified
int has_label(char* rawline, char* name_out){
    char line[linesize];
    sclean(rawline, line);
    if (smatch(line, "LABEL") || line[strlen(line)-1] == ':') {
        // labels are stored as a constant with the value being their line number

        // puts the label into constants[const_count]
        char labelname[linesize];
        int label_index = 0;
        ssplit(rawline, NULL, &label_index, "LABEL ");
        sclean(s_slice(rawline, label_index), labelname);
        sclean_i(labelname, name_out, ':', 0);
        return 1;
    } else {
        return 0;
    }
}

// returns 0 if no label found, 1 if a label is successfully identified
int has_label(f_string& rawline, f_string& name_out){
    f_string line = rawline.sclean();
    if (line.smatch("LABEL") || (line.length() > 0 && line[line.length()-1] == ':')) {
        // labels are stored as a constant with the value being their line number

        // puts the label into constants[const_count]
        f_string labelname;
        int label_index = 0;
        rawline.ssplit(NULL, &label_index, "LABEL ");
        labelname = rawline.slice(label_index).sclean();
        name_out = labelname.sclean_i(':', 0);
        return 1;
    } else {
        return 0;
    }
}

void build_relative_path(const char *input_file, f_string relative_file, f_string& out) {
    f_string path_copy = input_file;
    path_copy += '\0';

    out = dirname(path_copy);
    out += "/";
    out += relative_file; 
}


int macro_scan(array_vector<macro_def>& macros, array_vector<f_string>& constants, array_vector<int>& const_values, FILE* file, const char* path) {
    
    f_string rawline;
    bool top = file == input;
    while(1) {
        if(rawline.fgets(linesize, file) == NULL) {
            break;
        }
        if (rawline.smatch("INCLUDE ")) {
            f_string newfile = rawline.slice("INCLUDE ").sclean();
            f_string newfile_relative;
            FILE* included;
            build_relative_path(path, newfile, newfile_relative);
            included = fopen(newfile_relative, "r");
            if (!included) {
                return compile_error("failed to open file '{}'", newfile);
            }
            if (macro_scan(macros, constants, const_values, included, path) == -1) return -1;
            fclose(included);
        } else if (rawline.smatch("START_MACRO ")) {
            // grabs macro name
            int macro_scan_index = 0;
            rawline.slice("START_MACRO ").ssplit(macros.expand().macro_name, &macro_scan_index, " ");

            // grabs arg_count
            f_string macro_buffer;
            rawline.slice("START_MACRO ").ssplit(macro_buffer, &macro_scan_index, " ");
            macros().arg_count = atoi(macro_buffer);

            // grabs arg names
            f_string arg_name;
            for (int macro_arg_loop = 0; macro_arg_loop < macros().arg_count; macro_arg_loop++) {
                rawline.slice("START_MACRO ").ssplit(arg_name, &macro_scan_index, " ");
                macros.top().arg_names += arg_name.sclean();
            }

            int line_count = -1;
            while(1) {
                if(rawline.fgets(linesize, file) == NULL) {
                    return compile_error("no END_MACRO call found for {}", macros.top().macro_name);
                }
                if(rawline.smatch("END_MACRO ") && rawline.slice("END_MACRO ").smatch(macros.top().macro_name)) {
                    break;
                }
                line_count++;
                macros.top().body += rawline;
            }
            macros().line_count = line_count + 1;

            if(pverbose) std::print("macro name: {}, arg_count: {}, line_count: {}\n", macros().macro_name, macros().arg_count, macros().line_count);
            if(pverbose) std::print("macro count: {}\n\n", macros.get_count());
            
        } else if (rawline.smatch("DEFINE ") && !top) {
            int const_index = strlen("DEFINE ");

            rawline.ssplit(constants.expand(), &const_index, " ");

            // extracts the constant value as a string into const_val
            f_string const_val = rawline.slice(const_index).sclean();

            // parses the number in const_val and either puts the number into const_values or throws an error
            int imm;
            if ((imm = parse_number(const_val)) != -1) {
                if (imm < target->constants->imm_limit){
                    const_values += imm;
                    if (pverbose) std::print("Preprocessor: Constant '{}' defined as {}\n", constants.top(), const_values.top()); 
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
    rewind(file);
    return 0;
}