#define opcount 6
#define opcode_size 3
#define reg_count 3
#define instruction_size 16
#define imm_limit 32768

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