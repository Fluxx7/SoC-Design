// INSTRUCTION LAYOUTS

// n = unused
// r = input register 1
// t = input register 2
// o = output register
// i = instruction
// l = instruction type
// x = immediate
// s = swap bit (x-type only)

#define otype "llnrrrrrtttttnnnnnnnnnoooooiiiii"
#define itype "llnrrrrrxxxxxxxxxxxxxxxxxxxxxxxx"
#define xtype "llsrrrrrxxxxxxxxxxxxxxoooooiiiii"

// REGISTERS

// zero register
#define rz 0000

// generic registers
#define r0 0001
#define r1 0010
#define r2 0011
#define r3 0100
#define r4 0101
#define r5 0110
#define r6 0111

// function inputs
#define i0 1000
#define i1 1001
#define i2 1010
#define i3 1011

// return address
#define ir 1100

// return value
#define ra 1101

// extra registers (for now)
#define rs 1110
#define rc 1111

// OPERATIONS

// o-type

// logical
#define and 00000
#define or 00001
#define xor 00010
#define inv 00011

// unsigned arithmetic
#define add 00100
#define sub 00101
#define ushl 00110
#define ushr 00111

// signed arithmetic
#define sshl 01110
#define sshr 01111

// CONTROL

// branch

#define JGT 10001
#define JEQ 10010
#define JGE 10011
#define JLT 10100
#define JNE 10101
#define JLE 10110
#define JMP 10111

// stack

#define push 11000
#define pop 11001

#define call 11110
#define ret 11111
