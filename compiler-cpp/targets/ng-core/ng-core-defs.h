#include "../targetdef.h"

#define ng_opcount 6
#define ng_opcode_size 3
#define ng_reg_count 3
#define ng_instruction_size 16
#define ng_imm_limit 32768

#pragma once
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
