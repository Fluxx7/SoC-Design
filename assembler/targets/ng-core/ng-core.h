#pragma once
#include "ng-core-defs.h"


const char* ng_operators[ng_opcount] = { 
    "+",
    "-",
    "~",
    "^",
    "&",
    "|"
};
const char* ng_op_outs[ng_opcount] = {
    "ADD",
    "SUB",
    "INV",
    "XOR",
    "AND",
    "LOR"
};

const char* ng_opbits[ng_opcount] = {
    "100",
    "110",
    "011",
    "010",
    "000",
    "001"
};

const char* ng_branches[7] = {
    "JMP",
    "JLT",
    "JLE",
    "JEQ",
    "JGE",
    "JGT",
    "JNE"
};

const char* ng_branch_bits[7] = {
    "111",
    "100",
    "110",
    "010",
    "011",
    "001",
    "101"
};

const char* ng_registers[ng_reg_count] = {
    "A",
    "D",
    "*A"
};

const char* ng_regtok[ng_reg_count] = {
    "A",
    "D",
    "P"
};

const enum bitval ng_outbits[ng_reg_count] = {
    OUT_A,
    OUT_D,
    OUT_APTR
};

