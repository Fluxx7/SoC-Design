#ifndef FILE
    #include <stdio.h>
#endif
#include "../targets/targetdef.h"
#pragma once
int preprocess(struct comp_target* target, FILE* input, FILE* processed_input, FILE* statements, int mirror, int verbose);