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
    int preflect = 0;
    int verbose = 0;
    int pverbose = 0;
    struct comp_target* target = &targets[0];
    if (argc < 3){
        printf("An input and output file must both be provided\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
        if (strcmp(argv[i], "-m") == 0) {
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

    int instruction_type;
    int instruction_size = target->constants->instruction_size;
    preprocess(target, input, processed_input, statements, preflect, pverbose);
    rewind(input);
    char lineout[instruction_size+1];
    char rawline[linesize];
    for (int i = 0; i < instruction_size; i++) {
        lineout[i] = '0';
    }
    lineout[instruction_size] = '\0'; // CI x x * x u op1 op2 zx sw a d *a lt eq gt
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
                    int imm;
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
                    } else if ((imm = atoi(operation)) > 0){
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
                            // scan for labels matching the operation
                            long pos = ftell(processed_input);
                            rewind(processed_input);
                            char scanline[linesize];
                            char csline[linesize];
                            int label_found = 0;
                            int local_linenum = 0;
                            while(1){
                                if (fgets(scanline, linesize, processed_input) == NULL) {
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
                                    
                                } else {
                                    //printf("%s\n",csline);
                                    local_linenum++;
                                }
                            }
                            if (!label_found) {
                                return compile_error("Invalid operation");
                            }
                            fseek(processed_input, pos, SEEK_SET);
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
    while ((a = fgetc(output)) != EOF){
        fputc(a, outfile); 
    }
    printf("%s compiled successfully, saved to %s\n", argv[1], argv[2]);
    fclose(output);
    fclose(outfile);
    return 0;
}




