#pragma once
struct comp_target_constants {
    int opcount;
    int opcode_size;
    int reg_count;
    int instruction_size;
    int imm_limit;
};
struct comp_target {
    char name[6];
    int (*comp_function)(FILE*, char*);
    const char** const oplist[3];
    const char** const registers[2];
    const char** const branches[2];
    struct comp_target_constants *constants;
};