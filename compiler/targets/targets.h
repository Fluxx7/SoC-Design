#ifndef FILE
    #include <stdio.h>
#endif
#include "ng-core/ng-core-target.h"

int (*function)(FILE*, char*);

struct comp_target {
    char name[6];
    int (*comp_function)(FILE*, char*);
};

#define target_count 1

struct comp_target targets[target_count] = {
    {"ngc", &ng_code_gen}
};