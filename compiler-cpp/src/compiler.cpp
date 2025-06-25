#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>

#include "utilities/computils.hpp"
#include "preprocessor/preprocessor.hpp"
#include "../targets/targets.h"
#include "compiler.hpp"

// I don't care if globals are bad practice, functions that have to take 8 million variables
// to have access to every setting I want this compiler to have are worse
char rline_ptr[linesize];
int linenum = 0;
int truenum = 0;
FILE* mirror;
FILE* input;
FILE* output;
FILE* processed_input;
FILE* statements;
FILE* tokens;

int main(int argc, char** argv) {
    if (argc < 3){
        if (argc > 1 && strcmp(argv[1], "clean") == 0) {
            remove("macro_processing.txt");
            remove("procmirror.txt");
            remove("smirror.txt");
            remove("mirror.txt");
            std::println("Removed mirror files");
            return 0;
        } else {
            std::println("An input and output file must both be provided");
            return 1;
        } 
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' ){
            for (auto option : options) {
                if (option.smat && smatch(argv[i] + 1, option.tag)) {
                    if (option.opt_action(argv[i]) == 1) {
                        return 1;
                    }
                } else if (!option.smat && strcmp(argv[i] + 1, option.tag) == 0){
                    if (option.opt_action(argv[i]) == 1) {
                        return 1;
                    }
                }
            }
        }
    }
    
    input = fopen(argv[1], "r");
    output = tmpfile();
    processed_input = tmpfile(); // used to store  with 
    statements = tmpfile(); // used to store 
    tokens = tmpfile(); // used to store parsed tokens from instructions ie. output, X, Y, operation, jmp condition, in a way that is easily turned to machine code

    

    if (output == NULL){
        return compile_error("Failed to create temp file for compilation");
    }

    int instruction_type;
    int instruction_size = target->constants->instruction_size;
    if (preprocess(argv[1])) {
        return 1;
    }

    // If the compiler is only preprocessing the file
    if (pronly) {
        FILE* outfile = fopen(argv[2], "w");
        int a;
        while ((a = fgetc(processed_input)) != EOF){
            fputc(a, outfile); 
        }
        std::println("{} compiled successfully, saved to {}", argv[1], argv[2]);
        fclose(input);
        fclose(outfile);
        return 0;
    }


    if (reflect) mirror = fopen("mirror.txt", "w+");
    rewind(input);
    char* lineout = new char[instruction_size+1];
    f_string rawline;
    for (int i = 0; i < instruction_size; i++) {
        lineout[i] = '0';
    }
    lineout[instruction_size] = '\0';
    while (1) {

        f_string line;
        if(rawline.fgets(linesize, processed_input) == NULL){
            break;
        }
        int linenum_index = 0;
        rawline.ssplit(NULL, &linenum_index, " ");
        truenum = atoi(rawline.slice(linenum_index).get_content());
        rawline[linenum_index] = '\0';
        line = rawline.sclean();
        strncpy(rline_ptr, line.get_content(), linesize);
        if (line[0] != '#' && line.length() != 0){
            if (verbose) std::println("\nLine: {}", line);
            int i_split = 0;
            
            
            f_string outregs;
            // determine instruction type
            if (line.ssplit(outregs, &i_split,"=") == 0) {
                if (line.smatch("LABEL")){ 
                    continue;
                } else if (line.ssplit(outregs, &i_split,";") != 0){
                    instruction_type = 2;
                    i_split = 0;
                } else if (line.smatch( "JMP")) {
                    instruction_type = 0;
                } else {
                    return compile_error("Invalid instruction format");
                }
            } else {
                instruction_type = 1;
            } 

            // Register assignment
            if (instruction_type == 1) {
                f_string register_str;
                
                register_str += outregs;
                if (verbose) std::println("Register assignments: [{}]", register_str);
                int charnum = 0;
                int end = 0;
                pmirror("OUT ", tokens);
                while (!end) {
                    if(register_str[charnum] == (char) 0){
                        end = 1;
                    } else {
                        int found = 0;
                        for (int regname = 0; regname < target->constants->reg_count && !found; regname++){
                            if (target->registers[0][regname][0] == register_str[charnum]) {
                                if (strlen(target->registers[0][regname]) > 1) {
                                    int full_match = 1;
                                    for (int len = 1; len < strlen(target->registers[0][regname]) && full_match; len++) {
                                        if (target->registers[0][regname][len] != register_str[charnum+len]){
                                            full_match = 0;
                                        }
                                    }

                                    if (full_match) {
                                        pmirror(target->registers[1][regname], tokens);
                                        charnum += strlen(target->registers[0][regname]);
                                        found = 1;
                                    } else {
                                        return compile_error("invalid '*' in register assignment");
                                    }
                                    
                                } else {
                                    pmirror(target->registers[1][regname], tokens);
                                    charnum ++;
                                    found = 1;
                                }
                            }
                        }
                        if (found == 0) {
                            return compile_error("invalid character '{}' in register assignment", register_str[charnum]);
                        }
                    }
                }
            }
            pmirror("\n", tokens);
            if(verbose) std::println("instruction type {}", instruction_type);
            // Operation handling
            if (instruction_type != 0) {
                f_string operation;
                f_string token;
                if (line.ssplit(token, &i_split, ";") == 0){
                    operation += line.slice(i_split);
                } else {
                    operation = token;
                    instruction_type = 0;
                }

                pmirror("OP ", tokens);
                f_string opr;
                int op_ind = 0;
                int op_found = 0;
                for (int op = 0; op < target->constants->opcount && !op_found; op++){
                    if (operation.ssplit(opr, &op_ind, target->oplist[0][op]) != 0) {
                        pmirror(target->oplist[1][op],tokens);
                        pmirror("\n", tokens);
                        if (opr != "") {
                            for (int r = 0; r < target->constants->reg_count; r++){
                                if (opr == target->registers[0][r]) {
                                    pmirror("X ", tokens);
                                    pmirror(target->registers[1][r], tokens);
                                    pmirror("\n", tokens);
                                }
                            }
                        } else {
                            pmirror("X ", tokens);
                            pmirror("0", tokens);
                            pmirror("\n", tokens);
                        }
                        if (operation.slice(op_ind) != "") {
                            int reg_found = 0;
                            for (int r = 0; r < target->constants->reg_count; r++){
                                if (operation.slice(op_ind) == target->registers[0][r]) {
                                    pmirror("Y ", tokens);
                                    pmirror(target->registers[1][r], tokens);
                                    pmirror("\n", tokens);
                                    reg_found = 1;
                                }
                            }
                            if (!reg_found) {
                                if (parse_number(operation.slice(op_ind)) != -1) {
                                    pmirror("Y ", tokens);
                                    pmirror(operation.slice(op_ind), tokens);
                                    pmirror("\n", tokens);
                                }
                            }
                        }
                        if (verbose) std::println("pre-op: {} op: {} post-op: {}", opr, target->oplist[0][op], operation.slice(op_ind));
                        op_found = 1;
                    }
                }
                if (!op_found){
                    f_string buffer;
                    int imm = 0;
                    if (operation[0] == '0' && operation[1] == 'x'){
                        if ((imm = strtol(operation.slice(2).get_content(), NULL, 16)) > 0) {
                            if (imm < target->constants->imm_limit){
                                if (verbose) std::println("op is hex immediate of value {}", imm);
                                pmirror("IMM\n", tokens);
                                pmirror("X ", tokens);
                                buffer = std::format("{}\n", imm).c_str();
                                pmirror(buffer.get_content(), tokens);
                            }
                        }
                    } else if(operation[0] == '0' && operation[1] == 'b'){
                        f_string bin_num = operation.slice(2).sclean_i('_', 0);
                        if ((imm = strtol(bin_num.get_content(), NULL, 2)) > 0) {
                            if (imm < 32768){
                                if (verbose) std::println("op is binary immediate of value {}", imm);
                                pmirror("IMM\n", tokens);
                                pmirror("X ", tokens);
                                buffer = std::format("{}\n", imm).c_str();
                                pmirror(buffer.get_content(), tokens);
                            }  
                        }
                    } else if ((imm = atoi(operation.get_content())) > 0 || operation == "0"){
                        if (verbose) std::println("op is decimal immediate of value {}", imm);
                        pmirror("IMM\n", tokens);
                        pmirror("X ", tokens);
                        buffer = std::format("{}\n", imm).c_str();
                        pmirror(buffer.get_content(), tokens);
                    } else {
                        int reg_found = 0;
                        for (int i = 0; i < target->constants->reg_count && !reg_found; i++){
                            if (operation == target->registers[0][i]) {
                                if (verbose) std::println("op is copying value of {}", target->registers[0][i]);
                                reg_found = 1;
                                pmirror("MOV\n", tokens);
                                pmirror("X ", tokens);
                                pmirror(target->registers[1][i], tokens);
                                pmirror("\n", tokens);
                            }
                        }
                        if (!reg_found) {
                            return compile_error("Invalid operation");
                        }
                    }
                }
                if (verbose) std::println("Operation: {}\n", operation);
            }

            // handle branching
            if (instruction_type == 0){
                pmirror("J ", tokens);
                pmirror(line.slice(i_split).get_content(), tokens);
                pmirror("\n", tokens);
                if (verbose) std::println("Jump condition: {}", line.slice(i_split));
            }

            if (target->comp_function(tokens, lineout) == 1) {
                return 1;
            }

            fputs(lineout, output);
            if (debug) {
                f_string debug_line = rawline.sclean_i('\n', 0);
                fprintf(output, " %d %s", truenum, debug_line.get_content());
            }
            fputs("\n", output);
            if(reflect) fputs("\n\n", mirror);
            fclose(tokens);
            tokens = tmpfile();
            linenum++;

        }
    }


    fclose(input);
    if(reflect) fclose(mirror);

    // copy compiled code from temp file to final file
    rewind(output);
    FILE* outfile = fopen(argv[2], "w");
    int a;
    if (debug) {
        fputs("<debug>\n", outfile);
    }
    while ((a = fgetc(output)) != EOF){
        fputc(a, outfile); 
    }
    std::println("{} compiled successfully, saved to {}", argv[1], argv[2]);
    fclose(output);
    fclose(outfile);
    return 0;
}




