#define opcount 8
#define opcode_size 3
#define reg_count 3
#define instruction_size 16
#define imm_limit 32768

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

enum bitval {
    NONE = -1,
    CI = 0,
    PTR_IN = 3,
    OPCODE = 5,
    ZERO_X = 8,
    SWAP_XY = 9,
    OUT_A = 10,
    OUT_D = 11,
    OUT_APTR = 12,
    BRANCH = 13
};

const enum bitval outbits[reg_count] = {
    OUT_A,
    OUT_D,
    OUT_APTR
};