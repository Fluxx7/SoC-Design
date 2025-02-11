#ifndef FILE
    #include <stdio.h>
#endif

#ifndef opcount
    #include "ng-core-defs.h"
#endif

extern const char* operators[opcount];
extern const char* op_outs[opcount];
extern const char* opbits[opcount];

extern const char* branches[7];
extern const char* branch_bits[7];

extern const char* registers[reg_count];
extern const char* regtok[reg_count];

extern const enum bitval outbits[reg_count];

int ng_code_gen(FILE* tokens, char* lineout);