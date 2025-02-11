#ifndef opcount
    #include "ng-core-defs.h"
#endif

const char* operators[opcount] = { 
    "+1",
    "-1",
    "+",
    "-",
    "~",
    "^",
    "&",
    "|"
};
const char* op_outs[opcount] = {
    "AON",
    "SON",
    "ADD",
    "SUB",
    "INV",
    "XOR",
    "AND",
    "LOR"
};

const char* opbits[opcount] = {
    "101",
    "111",
    "100",
    "110",
    "011",
    "010",
    "000",
    "001"
};

const char* branches[7] = {
    "JMP",
    "JLT",
    "JLE",
    "JEQ",
    "JGE",
    "JGT",
    "JNE"
};

const char* branch_bits[7] = {
    "111",
    "100",
    "110",
    "010",
    "011",
    "001",
    "101"
};

const char* registers[reg_count] = {
    "A",
    "D",
    "*A"
};

const char* regtok[reg_count] = {
    "A",
    "D",
    "P"
};

const enum bitval outbits[reg_count] = {
    OUT_A,
    OUT_D,
    OUT_APTR
};

