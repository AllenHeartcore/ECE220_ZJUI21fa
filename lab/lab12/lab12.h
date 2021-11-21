#if !defined(LAB12_H)
#define LAB12_H

#include <stdint.h>

// type definitions for structures
typedef struct instruction_t instruction_t;
typedef struct code_stat_t   code_stat_t;
typedef struct basic_block_t basic_block_t;

typedef enum {
    INST_FLAG_NO_FLAGS = 0,		// symbolic name for no flags

    INST_FLAG_HALT = 1,			// is a HALT instruction
    INST_FLAG_RET  = 2,			// is a RET instruction

    // NEW FOR LAB 12
    INST_FLAG_TRAP = 4,			// instruction is a TRAP (not HALT)

    INST_FLAG_BAD_BR_TGT = 8,		// branch target outside of code
    INST_FLAG_BAD_JSR_TGT = 0x10,	// JSR target outside of code
    INST_FLAG_UNKNOWN_JSR_TGT = 0x20,	// JSR target unknown
    INST_FLAG_LEAVES_CODE = 0x40,	// next PC is outside of code

    // NEW FOR LAB 12
    INST_FLAG_ENDS_BASIC_BLOCK = 
        (INST_FLAG_HALT | INST_FLAG_RET | INST_FLAG_TRAP |
	 INST_FLAG_BAD_BR_TGT | INST_FLAG_BAD_JSR_TGT | 
	 INST_FLAG_UNKNOWN_JSR_TGT | INST_FLAG_LEAVES_CODE),

    INST_FLAG_START_SUBROUTINE = 0x80,	// start of a subroutine (a JSR target)
    INST_FLAG_SUB_CALLED = 0x100,       // subroutine called from main code
    INST_FLAG_SUB_EXPLORED = 0x200,     // subroutine checked already

    // for searches
    INST_FLAG_FOUND = 0x400,

    // NEW FOR LAB 12
    BLOCK_FLAG_IN_MULTIPLE = 0x800,	// block in multiple code sections
    BLOCK_FLAG_PRINTED = 0x1000,  	// block printed

} inst_flag_t;

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

    // NEW FOR LAB 12
    basic_block_t* in_block;		// basic block containing 
    					//    this instruction
};

struct code_stat_t {
    const char*    name;	// file name
    int32_t        len;		// number of 16-bit words
    uint16_t       start;	// starting address
    uint16_t* 	   bits;	// array of length len with 
    				//    bits for instructions
    instruction_t* inst;	// array of length len with instructions
};

struct basic_block_t {
    uint32_t	    uid;		// unique id, starting from 0
    code_stat_t*    code_stat;		// block from what code?
    uint16_t	    start_addr;		// first address in code including
    					//    this block (start of program,
					//    or start of a subroutine)
    int32_t         n_inst;		// number of instructions
    int32_t         max_inst;		// length of instruction array
    instruction_t** instruct;		// dynamically resized array of
    					//    instructions
    inst_flag_t     flags;		// flags for block (from last inst)
    uint16_t        n_incoming_arcs;	// number of basic blocks with
    					//    this block as possible
					//    next block
    basic_block_t*  next_block[2];	// next blocks, if any
};

extern inst_flag_t do_search (code_stat_t* cs, int32_t first);
extern basic_block_t* build_blocks (code_stat_t* cs, int32_t first, 
				    uint16_t block_start);

#endif // LAB12_H
