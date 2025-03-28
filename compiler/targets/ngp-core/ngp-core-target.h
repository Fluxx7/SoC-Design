#pragma once
#ifndef FILE
    #include <stdio.h>
#endif

#ifndef opcount
    #include "ngp-core-defs.h"
#endif

extern const char* ngp_operators[ngp_opcount];
extern const char* ngp_op_outs[ngp_opcount];
extern const char* ngp_opbits[ngp_opcount];

extern const char* ngp_branches[7];
extern const char* ngp_branch_bits[7];

extern const char* ngp_registers[ngp_reg_count];
extern const char* ngp_regtok[ngp_reg_count];

extern struct comp_target_constants ngp_target_constants;

int ngp_code_gen(FILE* tokens, char* lineout);