#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ng-core.h"
#include "ng-core-target.h"
#include "../../src/utilities/computils.hpp"

struct comp_target_constants ng_target_constants = {
    ng_opcount, ng_opcode_size, ng_reg_count, ng_instruction_size, ng_imm_limit};

int ng_code_gen(FILE *tokens, char *lineout)
{
    rewind(tokens);
    strcpy(lineout, "1000000000000000");
    f_string codeline;
    f_string x_reg  = "NULL";
    f_string y_reg  = "NULL";
    f_string out_regs  = "NULL";
    f_string op  = "";
    f_string jmp = "NULL";
    while (1)
    {
        if (codeline.fgets(linesize, tokens) == NULL)
        {
            // code for output here

            // immediate value handling
            if (op == "IMM" && !(x_reg == "0" || x_reg == "1"))
            {
                if (jmp != "NULL")
                {
                    return compile_error("ng-core: jumps cannot be performed when assigning an immediate value");
                }
                if (out_regs != "A")
                {
                    return compile_error("ng-core: immediate values can only be written to the A register");
                }
                else
                {
                    if (x_reg == "NULL") {
                        return compile_error("fatal error: immediate write was requested but value was not found");
                    }
                    lineout[CI] = '0';
                    f_string binval;
                    itob(atoi(x_reg), binval);
                    strcpy(s_slice(lineout, 1), binval.get_content());
                }
            }
            else
            {
                // set output bits
                for (int l = 0; l < out_regs.length(); l++)
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
                if (op == "IMM")
                {
                    if (x_reg == "0")
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
                        if (op == ng_op_outs[o])
                        {
                            for (int ol = 0; ol < ng_opcode_size; ol++)
                            {
                                lineout[OPCODE + ol] = ng_opbits[o][ol];
                            }
                        }
                    }
                }

                // handle register copying
                if (op == "MOV")
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
                else if (op != "IMM")
                {
                    if (x_reg != "NULL" && y_reg != "NULL")
                    {
                        // needs to check for operations where a "register" is a number like A = A + 1 or *A = A - 1

                        // set register control bits
                        if (x_reg[0] == '0')
                        {
                            if (op == "INV") {
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
                            if (y_reg == "1")
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
                if (jmp != "NULL")
                {
                    int match_found = 0;
                    for (int l = 0; l < 7 && !match_found; l++)
                    {
                        if (jmp == ng_branches[l])
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

        f_string cline = codeline.sclean_i('\n', 0);

        // outputs
        if (cline.smatch("OUT "))
        {
            out_regs = cline.slice(4);
        }

        // operation
        if (cline.smatch("OP "))
        {
            op =  cline.slice(3);
        }
        if (cline.smatch("X "))
        {
            x_reg = cline.slice(2);
        }
        if (cline.smatch("Y "))
        {
            y_reg = cline.slice(2);
        }
        if (cline.smatch("J "))
        {
            jmp = cline.slice(2);
        }
    }
}