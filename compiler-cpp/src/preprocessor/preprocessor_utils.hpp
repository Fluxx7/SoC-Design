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


struct macro_def {
    macro_def() {
        this->macro_name = f_string();
        this->arg_names = array_vector<f_string>();
        this->body = array_vector<f_string>();
    }
    f_string macro_name;
    int arg_count;
    array_vector<f_string> arg_names;
    array_vector<f_string> body;
    int line_count;
};

int macro_replace(array_vector<macro_def>&, int, f_string, int, FILE*, FILE*, int, char*);
int has_label(char* rawline, char* name_out);
int has_label(f_string& rawline, f_string& name_out);
void build_relative_path(const char *input_file, f_string relative_file, f_string& out);
int macro_scan(array_vector<macro_def>& macros, array_vector<f_string>& constants, array_vector<int>& const_values, FILE*,  const char* path);