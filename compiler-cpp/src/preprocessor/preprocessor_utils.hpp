#ifndef FILE
    #include <stdio.h>
#endif
#ifndef linesize
    #include "../utilities/computils.hpp"
#endif
#include "../../targets/targetdef.h"
#pragma once

#define stmirror(string, file) fputs(string, file); if(preflect) fputs(string, state_mirror)
#define prmirror(string, file) fputs(string, file); if(preflect) fputs(string, proc_mirror)
#define exmirror(string, file) fputs(string, file); if(preflect) fputs(string, macr_mirror)

#define expand_constants(counter, name_list, val_list) \
counter++;\
if (counter == 0) {\
    name_list = (char**) malloc(sizeof(char*));\
    val_list = (int*) malloc(sizeof(int));\
} else {\
    name_list = (char**) realloc(name_list, sizeof(char*)*(counter+1));\
    val_list = (int*) realloc(val_list, sizeof(int)*(counter+1));\
} \
name_list[counter] = (char*) calloc(linesize, sizeof(char));

typedef struct macro_def {
    char macro_name[linesize];
    int arg_count;
    char** arg_names;
    char** body;
    int line_count;
} macro_def;

int macro_replace(array_vector<macro_def>&, int, char*, int, FILE*, FILE*, int, char*);
int has_label(char* rawline, char* name_out);