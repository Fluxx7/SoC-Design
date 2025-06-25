#include "../targets/targets.h"
#include "string.h"
#pragma once
int debug = 0;
int reflect = 0;
int preflect = 0;
int verbose = 0;
int pverbose = 0;
int pronly;
struct comp_target* target = &targets[0];

struct comp_option {
    const char* tag;
    bool smat = false;
    int (*opt_action)(char*);
};

comp_option options[] = {
    {
        "v",
        false,
        [](char* argv){verbose = 1; return 0;}
    },
    {
        "pronly",
        false,
        [](char* argv){pronly = 1; return 0;}
    },
    {
        "m",
        false,
        [](char* argv){reflect = 1; return 0;}
    },
    {
        "a",
        false,
        [](char* argv){verbose = 1; reflect = 1; return 0;}
    },
    {
        "pm",
        false,
        [](char* argv){preflect = 1; return 0;}
    },
    {
        "pv",
        false,
        [](char* argv){pverbose = 1; return 0;}
    },
    {
        "pa",
        false,
        [](char* argv){preflect = 1; pverbose = 1; return 0;}
    },
    {
        "ma",
        false,
        [](char* argv){preflect = 1; reflect = 1; return 0;}
    },
    {
        "va",
        false,
        [](char* argv){verbose = 1; pverbose = 1; return 0;}
    },
    {
        "aa",
        false,
        [](char* argv){verbose = 1; pverbose = 1; preflect = 1; reflect = 1; return 0;}
    },
    {
        "d",
        false,
        [](char* argv){debug = 1; return 0;}
    },
    {
        "t=",
        true,
        [](char* argv){
            for (int j = 0; j < target_count; j++){
                if(strcmp(argv + 3, targets[j].name) == 0) {
                    target = &targets[j];
                    return 0;
                }
            }
            printf("invalid compilation target '%s'\n", argv + 3);
            return 1;
        }
    }

};