#ifndef FILE
    #include <stdio.h>
#endif
#include "ng-core/ng-core-target.h"
#include "targetdef.h"
#pragma once

//int (*function)(FILE*, char*);

#define target_count 1

struct comp_target targets[target_count] = {
    { // ng_core target
        "ngc", 
        &ng_code_gen, 
        {ng_operators, ng_op_outs, ng_opbits}, 
        {ng_registers, ng_regtok}, 
        {ng_branches, ng_branch_bits}, 
        &ng_target_constants
    }
};