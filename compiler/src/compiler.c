#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "utilities/computils.h"
#include "preprocessor/preprocessor.h"
#include "../targets/targets.h"

// I don't care if globals are bad practice, functions that have to take 8 million variables
// to have access to every setting I want this compiler to have are worse
char rline_ptr[linesize];
int linenum = 0;
int truenum = 0;
int debug = 0;
FILE* mirror;
int reflect = 0;
int preflect = 0;
int verbose = 0;
int pverbose = 0;
int pronly;
struct comp_target* target = &targets[0];
FILE* input;
FILE* output;
FILE* processed_input;
FILE* statements;
FILE* tokens;

int main(int argc, char** argv) {
    if (argc < 3){
        if (strcmp(argv[1], "clean") == 0) {
            remove("macro_processing.txt");
            remove("procmirror.txt");
            remove("smirror.txt");
            remove("mirror.txt");
            printf("Removed mirror files\n");
            return 0;
        } else {
            printf("An input and output file must both be provided\n");
            return 1;
        } 
    }

    // I should really make this in a smarter fashion
    // Maybe looping over an array of structs, with each struct having a name (eg 'pronly') and a function pointer that takes no arguments and returns void?
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
        if (strcmp(argv[i], "-pronly") == 0) {
            pronly = 1;
        }
        if (strcmp(argv[i], "-m") == 0) {
            reflect = 1;
        }
        if (strcmp(argv[i], "-a") == 0) {
            verbose = 1;
            reflect = 1;
        }
        if (strcmp(argv[i], "-pm") == 0) {
            preflect = 1;
        }
        if (strcmp(argv[i], "-pv") == 0) {
            pverbose = 1;
        }
        if (strcmp(argv[i], "-pa") == 0) {
            preflect = 1;
            pverbose = 1;
        }
        if (strcmp(argv[i], "-ma") == 0) {
            preflect = 1;
            reflect = 1;
        }
        if (strcmp(argv[i], "-va") == 0) {
            verbose = 1;
            pverbose = 1;
        }
        if (strcmp(argv[i], "-aa") == 0) {
            preflect = 1;
            pverbose = 1;
            verbose = 1;
            reflect = 1;
        }
        if (smatch(argv[i], "-t=") == 1) {
            for (int j = 0; j < target_count; j++){
                if(strcmp(s_slice(argv[i], 3), targets[j].name) == 0) {
                    target = &targets[j];
                } else {
                    printf("invalid compilation target '%s'\n", s_slice(argv[i], 3));
                    return 1;
                }
            }
        }
        if (strcmp(argv[i], "-d") == 0) {
            debug = 1;
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
        printf("%s compiled successfully, saved to %s\n", argv[1], argv[2]);
        fclose(input);
        fclose(outfile);
        return 0;
    }


    if (reflect) mirror = fopen("mirror.txt", "w+");
    rewind(input);
    char lineout[instruction_size+1];
    char rawline[linesize];
    for (int i = 0; i < instruction_size; i++) {
        lineout[i] = '0';
    }
    lineout[instruction_size] = '\0';
    while (1) {

        char line[linesize];
        if(fgets(rawline, linesize, processed_input) == NULL){
            break;
        }
        int linenum_index = 0;
        ssplit(rawline, NULL, &linenum_index, " ");
        truenum = atoi(s_slice(rawline, linenum_index));
        rawline[linenum_index] = '\0';
        sclean(rawline, line);
        strcpy(rline_ptr, line);
        if (line[0] != '#' && strlen(line) != 0){
            if(line[strlen(line)]!='\n'){
                if (verbose) printf("\nLine: %s\n", line);
            } else {
                if (verbose) printf("\nLine: %s", line);
            }
            
            int i_split = 0;
            
            
            char outregs[linesize];
            // determine instruction type
            if (ssplit(line, outregs, &i_split,"=") == 0) {
                if (smatch(line, "LABEL")){ 
                    continue;
                } else if (ssplit(line, outregs, &i_split,";") != 0){
                    instruction_type = 2;
                    i_split = 0;
                } else if (smatch(line, "JMP")) {
                    instruction_type = 0;
                } else {
                    return compile_error("Invalid instruction format");
                }
            } else {
                instruction_type = 1;
            } 

            // Register assignment
            if (instruction_type == 1) {
                char register_str[30] = "";
                
                strcat(register_str, outregs);
                if (verbose) printf("Register assignments: [%s]\n", register_str);
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
                            return compile_error("invalid character '%c' in register assignment", register_str[charnum]);
                        }
                    }
                }
            }
            pmirror("\n", tokens);
            if(verbose) printf("instruction type %d\n", instruction_type);
            // Operation handling
            if (instruction_type != 0) {
                char operation[linesize];
                char token[linesize];
                if (ssplit(line, token, &i_split, ";") == 0){
                    strcpy(operation, line+i_split);
                } else {
                    strcpy(operation, token);
                    instruction_type = 0;
                    //printf("Token: %s Operation: %s\n", *token, operation);
                }

                pmirror("OP ", tokens);
                char opr[linesize];
                int op_ind = 0;
                int op_found = 0;
                for (int op = 0; op < target->constants->opcount && !op_found; op++){
                    if (ssplit(operation, opr, &op_ind, target->oplist[0][op]) != 0) {
                        pmirror(target->oplist[1][op],tokens);
                        pmirror("\n", tokens);
                        if (strcmp(opr,"") != 0) {
                            for (int r = 0; r < target->constants->reg_count; r++){
                                if (strcmp(opr, target->registers[0][r]) == 0) {
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
                        if (strcmp(s_slice(operation, op_ind),"") != 0) {
                            int reg_found = 0;
                            for (int r = 0; r < target->constants->reg_count; r++){
                                if (strcmp(s_slice(operation, op_ind), target->registers[0][r]) == 0) {
                                    pmirror("Y ", tokens);
                                    pmirror(target->registers[1][r], tokens);
                                    pmirror("\n", tokens);
                                    reg_found = 1;
                                }
                            }
                            if (!reg_found) {
                                if (parse_number(s_slice(operation, op_ind)) != -1) {
                                    pmirror("Y ", tokens);
                                    pmirror(s_slice(operation, op_ind), tokens);
                                    pmirror("\n", tokens);
                                }
                            }
                        }
                        if (verbose) printf("pre-op: %s op: %s post-op: %s\n", opr, target->oplist[0][op], s_slice(operation, op_ind));
                        op_found = 1;
                    }
                }
                if (!op_found){
                    char buffer[32];
                    int imm = 0;
                    if (operation[0] == '0' && operation[1] == 'x'){
                        if ((imm = strtol(operation+2, NULL, 16)) > 0) {
                            if (imm < target->constants->imm_limit){
                                if (verbose) printf("op is hex immediate of value %x\n", imm);
                                pmirror("IMM\n", tokens);
                                pmirror("X ", tokens);
                                sprintf(buffer, "%d\n", imm);
                                pmirror(buffer, tokens);
                            }
                        }
                    } else if(operation[0] == '0' && operation[1] == 'b'){
                        char bin_num[strlen(operation+2)];
                        sclean_i(operation+2, bin_num, '_', 0);
                        if ((imm = strtol(bin_num, NULL, 2)) > 0) {
                            if (imm < 32768){
                                if (verbose) printf("op is binary immediate of value %d\n", imm);
                                pmirror("IMM\n", tokens);
                                pmirror("X ", tokens);
                                sprintf(buffer, "%d\n", imm);
                                pmirror(buffer, tokens);
                            }  
                        }
                    } else if ((imm = atoi(operation)) > 0 || strcmp(operation, "0") == 0){
                        if (verbose) printf("op is decimal immediate of value %d\n", imm);
                        pmirror("IMM\n", tokens);
                        pmirror("X ", tokens);
                        sprintf(buffer, "%d\n", imm);
                        pmirror(buffer, tokens);
                    } else {
                        int reg_found = 0;
                        for (int i = 0; i < target->constants->reg_count && !reg_found; i++){
                            if (strcmp(operation, target->registers[0][i]) == 0) {
                                if (verbose) printf("op is copying value of %s\n", target->registers[0][i]);
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
                if (verbose) printf("Operation: %s\n", operation);
            }

            // handle branching
            if (instruction_type == 0){
                pmirror("J ", tokens);
                pmirror(s_slice(line, i_split), tokens);
                pmirror("\n", tokens);
                if (verbose) printf("Jump condition: %s\n", s_slice(line, i_split));
            }

            if (target->comp_function(tokens, lineout) == 1) {
                return 1;
            }

            fputs(lineout, output);
            if (debug) {
                char debug_line[linesize];
                sclean_i(rawline, debug_line, '\n', 0);
                fprintf(output, " %d %s", truenum, debug_line);
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
    printf("%s compiled successfully, saved to %s\n", argv[1], argv[2]);
    fclose(output);
    fclose(outfile);
    return 0;
}




