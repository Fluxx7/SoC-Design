#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ng-core.h"
#include "ng-core-target.h"
#include "../../src/computils.h"

int ng_code_gen(FILE* tokens, char* lineout){
    rewind(tokens);
    strcpy(lineout, "1000000000000000");
    int mov = 0;
    char codeline[linesize];
    while (1) {
        if (fgets(codeline, linesize, tokens) == NULL) {
            break;
        }
        char cline[linesize];
        sclean_i(codeline, cline, '\n', 0);
        // handle outputs
        if (smatch(cline, "OUT ")){
            for (int l = 0; l < strlen(s_slice(cline, 3)); l++) {
                for (int r = 0; r < reg_count; r++) {
                    if (s_slice(cline, 3)[l] == regtok[r][0]) {
                        lineout[outbits[r]] = '1';            
                    }
                }
            }
        }
        if (smatch(cline, "OP ")){
            for (int o = 0; o < opcount; o++) {
                if (strcmp(s_slice(cline, 3), op_outs[o]) == 0) {
                    for (int ol = 0; ol < opcode_size; ol++) {
                        lineout[OPCODE+ol] = opbits[o][ol];
                    }
                }
            }
            if (strcmp(s_slice(cline, 3), "IMM") == 0) {
                lineout[CI] = '0';
            }
            if (strcmp(s_slice(cline, 3), "MOV") == 0) {
                lineout[ZERO_X] = '1';
                lineout[OPCODE] = '1';
                mov = 1;
            }
        }
        if (smatch(cline, "X ")){
            if (lineout[CI] == '0') {
                char binval[16];
                itob(atoi(s_slice(cline, 2)), binval);
                strcpy(s_slice(lineout,1), binval);
            } else if (cline[2] == '0') {
                lineout[ZERO_X] = '1';
            } else if (mov == 1) {
                if (cline[2] == 'D'){
                    lineout[SWAP_XY] = '1';
                    
                }
                if (cline[2] == 'P') {
                    lineout[PTR_IN] = '1';
                }
            } else if (cline[2] != 'D') {
                lineout[SWAP_XY] = '1';
                if (cline[2] == 'P') {
                    lineout[PTR_IN] = '1';
                }
            }
        }
        if (smatch(cline, "Y ")){
            if (cline[2] != 'A'){
                if (cline[2] == 'P'){
                    if(lineout[SWAP_XY] == '1' && lineout[ZERO_X] != 1){
                        return compile_error("D must be an operand");
                    } 
                    lineout[PTR_IN] = '1';
                }
            } else if(lineout[SWAP_XY] == '1'){
                    return compile_error("D must be an operand");
            } 
            
        }
        if (smatch(cline, "J ")){
            if (lineout[0] == '0') {
                return compile_error("jumps cannot be performed with immediate values");
            }
            int match_found = 0;
            for (int l = 0; l < 7 && !match_found; l++) {
                if (strcmp(branches[l],s_slice(cline, 2)) == 0) {
                    match_found = 1;
                    for (int s = 0; s < 3; s++) {
                        lineout[BRANCH+s] = branch_bits[l][s];
                    }
                }
            }
        }
    }
    return 0;
}