#include "preprocessor.hpp"
#include "preprocessor_utils.hpp"
#include <stdlib.h>
#include <string.h>

#include "../utilities/computils.hpp"

// returns 0 if no label found, 1 if a label is successfully identified
int has_label(char* rawline, char* name_out){
    char line[linesize];
    sclean(rawline, line);
    if (smatch(line, "LABEL") || line[strlen(line)-1] == ':') {
        // labels are stored as a constant with the value being their line number

        // puts the label into constants[const_count]
        char labelname[linesize];
        int label_index = 0;
        ssplit(rawline, NULL, &label_index, "LABEL ");
        sclean(s_slice(rawline, label_index), labelname);
        sclean_i(labelname, name_out, ':', 0);
        return 1;
    } else {
        return 0;
    }
}