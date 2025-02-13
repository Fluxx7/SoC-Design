#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "computils.h"
#include "preprocessor.h"
#include "../targets/targets.h"


char rline_ptr[linesize];
int linenum = 0;
int truenum = 0;
FILE* mirror;

int main(int argc, char** argv) {
    int reflect = 0;
    int verbose = 0;
    int (*target_gen)(FILE*, char*) = targets[0].comp_function;
    if (argc < 3){
        printf("An input and output file must both be provided\n");
        return 1;
    }

    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = 1;
            }
            if (strcmp(argv[i], "-m") == 0) {
                reflect = 1;
            }
            if (smatch(argv[i], "-t=") == 1) {
                for (int j = 0; j < target_count; j++){
                    if(strcmp(s_slice(argv[i], 3), targets[j].name) == 0) {
                        target_gen = targets[j].comp_function;
                    } else {
                        printf("invalid compilation target '%s'\n", s_slice(argv[i], 3));
                        return 1;
                    }
                }
            }
        }
    }
    
    FILE* input = fopen(argv[1], "r");
    FILE* output = tmpfile();
    FILE* processed_input = tmpfile(); // used to store  with 
    FILE* statements = tmpfile(); // used to store 
    FILE* tokens = tmpfile(); // used to store parsed tokens from instructions ie. output, X, Y, operation, jmp condition, in a way that is easily turned to machine code

    if (reflect) mirror = fopen("mirror.txt", "w+");

    if (output == NULL){
        return compile_error("Failed to create temp file for compilation");
    }
    char rawline[linesize];
    char** constants;
    int* const_values;

    int const_count = -1;
    int instruction_type;
    preprocess(input, processed_input, statements, reflect, verbose);
    rewind(input);
    char lineout[instruction_size+1] = "0000000000000000"; // CI x x * x u op1 op2 zx sw a d *a lt eq gt
    while (1) {
        if(fgets(rawline, linesize, input) == NULL){
            break;
        }
        strcpy(rline_ptr, rawline);
        char line[linesize];
        sclean(rawline, line);
        truenum++;
        if (line[0] != '#' && strlen(line) != 0){
            // handle constants
            if (linenum == 0 && smatch(rawline, "DEFINE ")) {
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
                    if (imm < imm_limit){
                        const_values[const_count] = imm;
                        if (verbose) printf("Constant '%s' defined as %d\n", constants[const_count], const_values[const_count]); 
                    } else {
                        return compile_error("Constants must be between 1 and %d", imm_limit - 1);
                    }
                }
            } 
            // handle instructions
            else {
                if(rawline[strlen(rawline)]=='\n'){
                    if (verbose) printf("\nRaw line: %sCleaned line: %s\n", rawline, line);
                } else {
                    if (verbose) printf("\nRaw line: %s\nCleaned line: %s\n", rawline, line);
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
                    char register_str[30] = " ";
                    
                    strcat(register_str, outregs);
                    strcat (register_str, " ");
                    if (verbose) printf("Register assignments: %s \n", register_str);
                    int charnum = 0;
                    int end = 0;
                    pmirror("OUT ", tokens);
                    while (!end) {
                        if(register_str[charnum] == (char) 0){
                            end = 1;
                        } else {
                            switch (register_str[charnum])
                            {
                            case 'A':
                                pmirror("A", tokens);
                                charnum++;
                                break;
                            case 'D':
                                pmirror("D", tokens);
                                charnum++;
                                break;
                            case '*':
                                if (register_str[charnum + 1] == 'A') {
                                    pmirror("P", tokens);
                                    charnum += 2;
                                } else {
                                    return compile_error("invalid lone '*' in register assignment");
                                }
                                break;
                            case ' ':
                                charnum++;
                                break;
                            default:
                                return compile_error("invalid character '%c' in register assignment", register_str[charnum]);
                            }
                        }
                    }
                }
                pmirror("\n", tokens);

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
                    for (int op = 0; op < opcount && !op_found; op++){
                        if (ssplit(operation, opr, &op_ind, operators[op]) != 0) {
                            pmirror(op_outs[op],tokens);
                            pmirror("\n", tokens);
                            if (strcmp(opr,"") != 0) {
                                for (int r = 0; r < reg_count; r++){
                                    if (strcmp(opr, registers[r]) == 0) {
                                        pmirror("X ", tokens);
                                        pmirror(regtok[r], tokens);
                                        pmirror("\n", tokens);
                                    }
                                }
                            } else {
                                pmirror("X ", tokens);
                                pmirror("0", tokens);
                                pmirror("\n", tokens);
                            }
                            if (strcmp(s_slice(operation, op_ind),"") != 0) {
                                for (int r = 0; r < reg_count; r++){
                                    if (strcmp(s_slice(operation, op_ind), registers[r]) == 0) {
                                        pmirror("Y ", tokens);
                                        pmirror(regtok[r], tokens);
                                        pmirror("\n", tokens);
                                    }
                                }
                            }
                            if (verbose) printf("pre-op: %s op: %s post-op: %s\n", opr, operators[op], s_slice(operation, op_ind));
                            op_found = 1;
                        }
                    }
                    if (!op_found){
                        char buffer[32];
                        int imm;
                        int is_const = 0;
                        for (int m = 0; m < (const_count+1); m++) {
                            if (strcmp(operation, constants[m]) == 0) {
                                is_const = 1;
                                if (verbose) printf("op is const '%s' with value %d\n", constants[m], const_values[m]);
                                pmirror("IMM\n", tokens);
                                pmirror("X ", tokens);
                                sprintf(buffer, "%d", const_values[m]);
                                pmirror(buffer, tokens);
                                pmirror("\n", tokens);
                            }
                        }
                        if (!is_const) {
                            if (operation[0] == '0' && operation[1] == 'x'){
                                if ((imm = strtol(operation+2, NULL, 16)) > 0) {
                                    if (imm < 32768){
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
                            } else if ((imm = atoi(operation)) > 0){
                                if (verbose) printf("op is decimal immediate of value %d\n", imm);
                                pmirror("IMM\n", tokens);
                                pmirror("X ", tokens);
                                sprintf(buffer, "%d\n", imm);
                                pmirror(buffer, tokens);
                            } else {
                                int reg_found = 0;
                                for (int i = 0; i < reg_count && !reg_found; i++){
                                    if (strcmp(operation, registers[i]) == 0) {
                                        if (verbose) printf("op is copying value of %s\n", registers[i]);
                                        reg_found = 1;
                                        pmirror("MOV\n", tokens);
                                        pmirror("X ", tokens);
                                        pmirror(regtok[i], tokens);
                                        pmirror("\n", tokens);
                                    }
                                }
                                if (!reg_found) {
                                    // scan for labels matching the operation
                                    long pos = ftell(input);
                                    rewind(input);
                                    char scanline[linesize];
                                    char csline[linesize];
                                    int label_found = 0;
                                    int local_linenum = 0;
                                    while(1){
                                        if (fgets(scanline, linesize, input) == NULL) {
                                            break;
                                        }
                                        sclean(scanline, csline);
                                        if (smatch(csline, "LABEL")){
                                            if (strcmp(s_slice(csline, 5),operation) == 0) {
                                                label_found = 1;
                                                if (verbose) printf("op is using label '%s' at linenum %d\n", operation, local_linenum); 
                                                pmirror("IMM\n", tokens);
                                                pmirror("X ", tokens);
                                                sprintf(buffer, "%d\n", local_linenum);
                                                pmirror(buffer, tokens);
                                                break;
                                            }
                                            
                                        } else if (csline[0] != '#' && strlen(csline) != 0 && smatch(csline, "DEFINE") != 1) {
                                            //printf("%s\n",csline);
                                            local_linenum++;
                                        }
                                    }
                                    if (!label_found) {
                                        return compile_error("Invalid operation");
                                    }
                                    fseek(input, pos, SEEK_SET);
                                }
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

                if (target_gen(tokens, lineout) == 1) {
                    return 1;
                }

                fputs(lineout, output);
                fputs("\n", output);
                if(reflect) fputs("\n\n", mirror);
                fclose(tokens);
                tokens = tmpfile();
                linenum++;
            }

        }
    }
    // releasing all allocated memory
    if (const_count != -1) {
        for (int i = 0; i <= const_count; i++) {
            free(constants[i]);
        }
        free(constants);
        free(const_values);
    }


    fclose(input);
    if(reflect) fclose(mirror);

    // copy compiled code from temp file to final file
    rewind(output);
    FILE* outfile = fopen(argv[2], "w");
    int a;
    while ((a = fgetc(output)) != EOF){
        fputc(a, outfile); 
    }
    printf("%s compiled successfully, saved to %s\n", argv[1], argv[2]);
    fclose(output);
    fclose(outfile);
    return 0;
}




