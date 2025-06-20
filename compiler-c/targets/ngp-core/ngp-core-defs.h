#include "../targetdef.h"

#define ngp_opcount 6
#define ngp_opcode_size 3
#define ngp_reg_count 8
#define ngp_instruction_size 16
#define ngp_imm_limit 32768

#pragma once
enum bitval {
    NONE = -1,
    CI = 0,
    PTR_X = 1,
    RX = 2,
    RY = 4,
    PTR_OUT = 6,
    OUT = 7,
    OPCODE = 9,
    UNKNOWN = 12,
    BRANCH = 13
};
