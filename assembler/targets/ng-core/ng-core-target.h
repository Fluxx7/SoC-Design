#pragma once
#ifndef FILE
    #include <stdio.h>
#endif

#ifndef opcount
    #include "ng-core-defs.h"
#endif

extern const char* ng_operators[ng_opcount];
extern const char* ng_op_outs[ng_opcount];
extern const char* ng_opbits[ng_opcount];

extern const char* ng_branches[7];
extern const char* ng_branch_bits[7];

extern const char* ng_registers[ng_reg_count];
extern const char* ng_regtok[ng_reg_count];

extern const enum bitval ng_outbits[ng_reg_count];
extern struct comp_target_constants ng_target_constants;

int ng_code_gen(FILE* tokens, char* lineout);