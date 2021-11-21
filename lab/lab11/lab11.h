#if !defined(LAB11_H)
#define LAB11_H

#include <stdint.h>


typedef enum {
    INST_FLAG_NO_FLAGS = 0,		// symbolic name for no flags

    INST_FLAG_HALT = 1,			// is a HALT instruction
    INST_FLAG_RET  = 2,			// is a RET instruction

    INST_FLAG_BAD_BR_TGT = 4,		// branch target outside of code
    INST_FLAG_BAD_JSR_TGT = 8,		// JSR target outside of code
    INST_FLAG_UNKNOWN_JSR_TGT = 16,	// JSR target unknown
    INST_FLAG_LEAVES_CODE = 32,		// next PC is outside of code

    INST_FLAG_START_SUBROUTINE = 64,	// start of a subroutine (a JSR target)
    INST_FLAG_SUB_CALLED = 128,         // subroutine called from main code
    INST_FLAG_SUB_EXPLORED = 256,       // subroutine checked already

    // for searches
    INST_FLAG_FOUND = 512
} inst_flag_t;



typedef struct instruction_t instruction_t;
struct instruction_t {
    uint16_t       address;		// address of instruction
    uint16_t       n_incoming_arcs;	// number of instructions with
    					//    this instruction as possible
					//    next instruction
    instruction_t* next_inst;		// next in memory, if any
    					//    (and instruction is not BRnzp)
    instruction_t* branch_tgt;		// branch target, if any
    instruction_t* jsr_tgt;		// jsr target, if any
    inst_flag_t    flags;		// flags (see above)

    // USE THIS FIELD FOR YOUR DFS 'STACK' (built as a linked list)
    instruction_t* next;
};

typedef struct code_stat_t code_stat_t;
struct code_stat_t {
    const char*    name;	// file name
    int32_t        len;		// number of 16-bit words
    uint16_t       start;	// starting address
    uint16_t* 	   bits;	// array of length len with 
    				//    bits for instructions
    instruction_t* inst;	// array of length len with instructions
};

extern inst_flag_t do_search (code_stat_t* cs, int32_t first);

#endif // LAB11_H
