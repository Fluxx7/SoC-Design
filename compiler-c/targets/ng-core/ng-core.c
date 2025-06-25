#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ng-core.h"
#include "ng-core-target.h"
#include "../../src/utilities/computils.h"

struct comp_target_constants ng_target_constants = {
    ng_opcount, ng_opcode_size, ng_reg_count, ng_instruction_size, ng_imm_limit};

int ng_code_gen(FILE *tokens, char *lineout)
{
    rewind(tokens);
    strcpy(lineout, "1000000000000000");
    char codeline[linesize];
    char x_reg[32] = "NULL";
    char y_reg[8] = "NULL";
    char out_regs[8] = "NULL";
    char op[4] = "";
    char jmp[5] = "NULL";
    while (1)
    {
        if (fgets(codeline, linesize, tokens) == NULL)
        {
            // code for output here

            // immediate value handling
            if (strcmp(op, "IMM") == 0 && !(strcmp(x_reg, "0") == 0 || strcmp(x_reg, "1") == 0))
            {
                if (strcmp(jmp, "NULL") != 0)
                {
                    return compile_error("ng-core: jumps cannot be performed when assigning an immediate value");
                }
                if (strcmp(out_regs, "A") != 0)
                {
                    return compile_error("ng-core: immediate values can only be written to the A register");
                }
                else
                {
                    if (strcmp(x_reg, "NULL") == 0)
                    {
                        return compile_error("fatal error: immediate write was requested but value was not found");
                    }
                    lineout[CI] = '0';
                    char binval[17];
                    itob(atoi(x_reg), binval);
                    strcpy(s_slice(lineout, 1), binval);
                }
            }
            else
            {
                // set output bits
                for (int l = 0; l < strlen(out_regs); l++)
                {
                    for (int r = 0; r < ng_reg_count; r++)
                    {
                        if (out_regs[l] == ng_regtok[r][0])
                        {
                            lineout[ng_outbits[r]] = '1';
                        }
                    }
                }

                // set operation bits
                if (strcmp(op, "IMM") == 0)
                {
                    if (strcmp(x_reg, "0") == 0)
                    {
                        // sets the opcode to be '~x' and then zeros x
                        for (int ol = 0; ol < ng_opcode_size; ol++)
                        {
                            lineout[OPCODE + ol] = ng_opbits[4][ol];
                        }
                        lineout[ZERO_X] = '1';
                    }
                    else
                    {
                        // sets the opcode to be 'x + 1' and then zeros x
                        lineout[OPCODE] = '1';
                        lineout[OPCODE + 1] = '0';
                        lineout[OPCODE + 2] = '1';
                        lineout[ZERO_X] = '1';
                    }
                }
                else
                {
                    for (int o = 0; o < ng_opcount; o++)
                    {
                        if (strcmp(op, ng_op_outs[o]) == 0)
                        {
                            for (int ol = 0; ol < ng_opcode_size; ol++)
                            {
                                lineout[OPCODE + ol] = ng_opbits[o][ol];
                            }
                        }
                    }
                }

                // handle register copying
                if (strcmp(op, "MOV") == 0)
                {
                    lineout[ZERO_X] = '1';
                    lineout[OPCODE] = '1';
                    if (x_reg[0] == 'D')
                    {
                        lineout[SWAP_XY] = '1';
                    }
                    else if (x_reg[0] == 'P')
                    {
                        lineout[PTR_IN] = '1';
                    }
                }
                else if (strcmp(op, "IMM") != 0)
                {
                    if (strcmp(x_reg, "NULL") != 0 && strcmp(y_reg, "NULL") != 0)
                    {
                        // needs to check for operations where a "register" is a number like A = A + 1 or *A = A - 1

                        // set register control bits
                        if (x_reg[0] == '0')
                        {
                            if (strcmp(op, "INV") == 0) {
                                switch (y_reg[0])
                                {
                                case 'D':
                                    break;
                                case 'P':
                                    lineout[PTR_IN] = '1';
                                case 'A':
                                    lineout[SWAP_XY] = '1';
                                    break;
                                default:
                                    return compile_error("fatal error: invalid register provided as operand");
                                    break;
                                }
                                
                            } else {
                                lineout[ZERO_X] = '1';
                                switch (y_reg[0])
                                {
                                    case 'D':
                                        lineout[SWAP_XY] = '1';
                                        break;
                                    case 'A':
                                        break;
                                    case 'P':
                                        lineout[PTR_IN] = '1';
                                        break;
                                    default:
                                        return compile_error("fatal error: invalid register provided as operand");
                                        break;
                                }
                            }
                        }
                        else if ((x_reg[0] != 'D' && y_reg[0] != 'D') || y_reg[0] == '1')
                        {
                            if (strcmp(y_reg, "1") == 0)
                            {
                                lineout[OPCODE + 2] = '1';
                                switch (x_reg[0])
                                {
                                case 'P':
                                    lineout[PTR_IN] = '1';
                                case 'A':
                                    lineout[SWAP_XY] = '1';
                                case 'D':
                                    break;
                                default:
                                    return compile_error("fatal error: invalid register provided as operand");
                                    break;
                                }
                            }
                            else
                            {
                                return compile_error("ng-core: D must be an operand");
                            }
                        }
                        else if (x_reg[0] == y_reg[0])
                        {
                            return compile_error("ng-core: one register cannot be used for both operands");
                        }
                        else
                        {
                            if (x_reg[0] == 'D')
                            {
                                if (y_reg[0] == 'P')
                                {
                                    lineout[PTR_IN] = '1';
                                }
                            }
                            else
                            {
                                lineout[SWAP_XY] = '1';
                                if (x_reg[0] == 'P')
                                {
                                    lineout[PTR_IN] = '1';
                                }
                            }
                        }
                    }
                }

                // handle branches
                if (strcmp(jmp, "NULL") != 0)
                {
                    int match_found = 0;
                    for (int l = 0; l < 7 && !match_found; l++)
                    {
                        if (strcmp(ng_branches[l], jmp) == 0)
                        {
                            match_found = 1;
                            for (int s = 0; s < 3; s++)
                            {
                                lineout[BRANCH + s] = ng_branch_bits[l][s];
                            }
                        }
                    }
                }
            }

            return 0;
        }
        // code for reading inputs here

        char cline[linesize];
        sclean_i(codeline, cline, '\n', 0);

        // outputs
        if (smatch(cline, "OUT "))
        {
            strcpy(out_regs, s_slice(cline, 4));
        }

        // operation
        if (smatch(cline, "OP "))
        {
            strcpy(op, s_slice(cline, 3));
        }
        if (smatch(cline, "X "))
        {
            strcpy(x_reg, s_slice(cline, 2));
        }
        if (smatch(cline, "Y "))
        {
            strcpy(y_reg, s_slice(cline, 2));
        }
        if (smatch(cline, "J "))
        {
            strcpy(jmp, s_slice(cline, 2));
        }
    }
}