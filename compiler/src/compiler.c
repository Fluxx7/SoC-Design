#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "computils.h"
#include "preprocessor.h"
#include "../targets/targets.h"


char** rline_ptr;
int linenum = 0;
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
    char* rawline = (char*) calloc(linesize, (size_t) (linesize * sizeof(char)));
    
    //rline_ptr = &rawline;
    char** constants;
    int* const_values;

    int const_count = -1;
    int instruction_type;
    preprocess(input, processed_input, statements, reflect, verbose);
    char lineout[instruction_size+1] = "0000000000000000"; // CI x x * x u op1 op2 zx sw a d *a lt eq gt
    while (1) {
        if(fgets(rawline, linesize, input) == NULL){
            break;
        }
        char* line = sclean(rawline);
        if (line[0] != '#' && strlen(line) != 0){
            // handle constants
            if (linenum == 0 && smatch(rawline, "DEFINE ")) {
                int const_index = 7;
                char* const_name = ssplit(rawline, &const_index, " ");
                char* const_val = sclean(s_slice(rawline, const_index));
                if (const_count == -1) {
                    constants = (char**) malloc(sizeof(char*));
                    const_values = (int*) malloc(sizeof(int));
                } else {
                    constants = (char**) realloc(constants, sizeof(char*)*(const_count+2));
                    const_values = (int*) realloc(const_values, sizeof(int)*(const_count+2));
                }
                const_count++;
                constants[const_count] = const_name;
                int imm;
                if ((imm = parse_number(const_val)) != -1) {
                    if (imm < imm_limit){
                        const_values[const_count] = imm;
                        if (verbose) printf("Constant '%s' defined as %d\n", constants[const_count], const_values[const_count]); 
                    } else {
                        return compile_error("Constants must be between 1 and %d", imm_limit - 1);
                    }
                }
                free(const_val);
            } 
            // handle instructions
            else {
                if(rawline[strlen(rawline)]=='\n'){
                    if (verbose) printf("\nRaw line: %sCleaned line: %s\n", rawline, line);
                } else {
                    if (verbose) printf("\nRaw line: %s\nCleaned line: %s\n", rawline, line);
                }
                
                int i_split = 0;
                
                
                char* outregs;
                // determine instruction type
                if ((outregs = ssplit(line, &i_split,"=")) == NULL) {
                    if (smatch(line, "LABEL")){ 
                        free(line);
                        continue;
                    } else if ((outregs = ssplit(line, &i_split,";")) != NULL){
                        instruction_type = 2;
                        i_split = 0;
                        free(outregs);

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
                    free(outregs);
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
                    char* operation = (char*) calloc(linesize, (size_t) (linesize * sizeof(char)));
                    char* token;
                    if ((token = ssplit(line, &i_split, ";")) == NULL){
                        strcpy(operation, line+i_split);
                    } else {
                        strcpy(operation, token);
                        instruction_type = 0;
                        //printf("Token: %s Operation: %s\n", *token, operation);
                        free(token);
                    }

                    pmirror("OP ", tokens);
                    char* opr;
                    int op_ind = 0;
                    int op_found = 0;
                    for (int op = 0; op < opcount && !op_found; op++){
                        if ((opr = ssplit(operation, &op_ind, operators[op])) != NULL) {
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
                            free(opr);
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
                                char* bin_num = sclean_i(operation+2, '_', 0);
                                if ((imm = strtol(bin_num, NULL, 2)) > 0) {
                                    if (imm < 32768){
                                        if (verbose) printf("op is binary immediate of value %d\n", imm);
                                        pmirror("IMM\n", tokens);
                                        pmirror("X ", tokens);
                                        sprintf(buffer, "%d\n", imm);
                                        pmirror(buffer, tokens);
                                    }  
                                }
                                free(bin_num);
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
                                    char* scanline = (char*) calloc(linesize, (size_t) (linesize * sizeof(char)));
                                    char* csline;
                                    int label_found = 0;
                                    int local_linenum = 0;
                                    while(1){
                                        if (fgets(scanline, linesize, input) == NULL) {
                                            break;
                                        }
                                        csline = sclean(scanline);
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
                                        free(csline);
                                    }
                                    free(scanline);
                                    if (!label_found) {
                                        return compile_error("Invalid operation");
                                    }
                                    fseek(input, pos, SEEK_SET);
                                }
                            }
                        }
                    }
                    if (verbose) printf("Operation: %s\n", operation);
                    free(operation);
                }

                // handle branching
                if (instruction_type == 0){
                    pmirror("J ", tokens);
                    pmirror(s_slice(line, i_split), tokens);
                    pmirror("\n", tokens);
                    if (verbose) printf("Jump condition: %s\n", s_slice(line, i_split));
                }

                // turn tokens into machine code
                // rewind(tokens);
                // strcpy(lineout, "1000000000000000");
                // int mov = 0;
                // char* codeline = (char*) calloc(linesize, (size_t) (linesize * sizeof(char)));
                // while (1) {
                //     if (fgets(codeline, linesize, tokens) == NULL) {
                //         break;
                //     }
                //     char* cline = sclean_i(codeline, '\n', 0);
                //     // handle outputs
                //     if (smatch(cline, "OUT ")){
                //         for (int l = 0; l < strlen(s_slice(cline, 3)); l++) {
                //             for (int r = 0; r < reg_count; r++) {
                //                 if (s_slice(cline, 3)[l] == regtok[r][0]) {
                //                     lineout[outbits[r]] = '1';            
                //                 }
                //             }
                //         }
                //     }
                //     if (smatch(cline, "OP ")){
                //         for (int o = 0; o < opcount; o++) {
                //             if (strcmp(s_slice(cline, 3), op_outs[o]) == 0) {
                //                 for (int ol = 0; ol < opcode_size; ol++) {
                //                     lineout[OPCODE+ol] = opbits[o][ol];
                //                 }
                //             }
                //         }
                //         if (strcmp(s_slice(cline, 3), "IMM") == 0) {
                //             if (!smatch(s_slice(lineout,OUT_A), "100")) {
                //                 return compile_error("immediate values must be written to the A register exclusively");
                //             }
                //             lineout[CI] = '0';
                //         }
                //         if (strcmp(s_slice(cline, 3), "MOV") == 0) {
                //             lineout[ZERO_X] = '1';
                //             lineout[OPCODE] = '1';
                //             mov = 1;
                //         }
                //     }
                //     if (smatch(cline, "X ")){
                //         if (lineout[CI] == '0') {
                //             char binval[16];
                //             itob(atoi(s_slice(cline, 2)), binval);
                //             strcpy(s_slice(lineout,1), binval);
                //         } else if (cline[2] == '0') {
                //             lineout[ZERO_X] = '1';
                //         } else if (mov == 1) {
                //             if (cline[2] == 'D'){
                //                 lineout[SWAP_XY] = '1';
                                
                //             }
                //             if (cline[2] == 'P') {
                //                 lineout[PTR_IN] = '1';
                //             }
                //             //printf("rawline: '%c', lineout: %s\n", rawline[2], lineout);
                //         } else if (cline[2] != 'D') {
                //             lineout[SWAP_XY] = '1';
                //             if (cline[2] == 'P') {
                //                 lineout[PTR_IN] = '1';
                //             }
                //         }
                //     }
                //     if (smatch(cline, "Y ")){
                //         if (cline[2] != 'A'){
                //             if (cline[2] == 'P'){
                //                 if(lineout[SWAP_XY] == '1' && lineout[ZERO_X] != 1){
                //                     return compile_error("D must be an operand");
                //                 } 
                //                 lineout[PTR_IN] = '1';
                //             }
                //         } else if(lineout[SWAP_XY] == '1'){
                //                 return compile_error("D must be an operand");
                //         } 
                        
                //     }
                //     if (smatch(cline, "J ")){
                //         int match_found = 0;
                //         for (int l = 0; l < 7 && !match_found; l++) {
                //             if (strcmp(branches[l],s_slice(cline, 2)) == 0) {
                //                 match_found = 1;
                //                 for (int s = 0; s < 3; s++) {
                //                     lineout[BRANCH+s] = branch_bits[l][s];
                //                 }
                //             }
                //         }
                //     }
                //     free(cline);
                // }
                target_gen(tokens, lineout);

                fputs(lineout, output);
                fputs("\n", output);
                if(reflect) fputs("\n\n", mirror);
                fclose(tokens);
                tokens = tmpfile();
                linenum++;
            }

        }
        free(line);
    }
    // releasing all allocated memory
    free(rawline);
    for (int i = 0; i <= const_count; i++) {
        free(constants[i]);
    }
    free(constants);
    free(const_values);


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




