#pragma once
#include "ngp-core-defs.h"


const char* ngp_operators[ngp_opcount] = { 
    "+",
    "-",
    "~",
    "^",
    "&",
    "|"
};
const char* ngp_op_outs[ngp_opcount] = {
    "ADD",
    "SUB",
    "INV",
    "XOR",
    "AND",
    "LOR"
};

const char* ngp_opbits[ngp_opcount] = {
    "100",
    "110",
    "011",
    "010",
    "000",
    "001"
};

const char* ngp_branches[7] = {
    "JMP",
    "JLT",
    "JLE",
    "JEQ",
    "JGE",
    "JGT",
    "JNE"
};

const char* ngp_branch_bits[7] = {
    "111",
    "100",
    "110",
    "010",
    "011",
    "001",
    "101"
};

const char* ngp_registers[ngp_reg_count] = {
    "A",
    "D",
    "*A"
};

const char* ngp_regtok[ngp_reg_count] = {
    "A",
    "D",
    "P"
};


