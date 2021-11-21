#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab12.h"


basic_block_t* 
build_blocks (code_stat_t* cs, int32_t first, uint16_t block_start)
{
    static uint32_t next_id = 0;	// uid (used when allocating new blocks)
    basic_block_t*  bb = NULL;		// basic block
    instruction_t*  cur;		// current instruction
    instruction_t*  next;		// next instruction
    int32_t         n_next = 0;		// number of next blocks

    // First, set cur to point to the instruction starting the block
    // (the instruction with index 'first' in cs).

    // Then set bb to point to the current instruction's basic block,
    // which may still be NULL.

    // Is the current instruction already part of a basic block?
    if (0) { // (fix the condition)

	// Does the basic block's starting address correspond to block_start?
	// If not, the block was reached from another part of the code
	// (another subroutine, for example), and should be marked with
	// the BLOCK_FLAG_IN_MULTIPLE flag.

	// Return the current instruction's basic block.
    }

    // Allocate a new block and a space for a single instruction_t*
    // for the instruct array in the new block.  Be sure not to leak
    // memory if only one of the allocations succeeds.  The pointer
    // to the new basic block should be stored in the bb variable.

    // Set the current instruction's basic block to bb.

    // Initialize all remaining fields of the new basic block (only
    // instruct was initialized already).  Note that we have one instruction
    // (cur) and room for one instruction in the instruct array at this 
    // point.  Both next_block's should be set to NULL.  Flags and
    // incoming arcs should be copied from the last and first instructions
    // in the block, respectively, but both of those are the current
    // instruction at this point.

    // Set the next instruction to point to the next_inst field of the
    // current instruction (it may be NULL, of course).

    // Loop and add more instructions to the basic block until one of 
    // the following is true:
    // 1.  Current instruction's flags include a flag that ends a
    //     basic block (use INST_FLAG_ENDS_BASIC_BLOCK to check).
    // 2.  The current instruction has a branch target.
    // 3.  The current instruction has a JSR target.
    // 4.  The next instruction is NULL.
    // 5.  The next instruction has >1 incoming arc.
    while (0) { // fix the conditions as above

	// Use dynamic resizing to make sure that there is space
	// for a new instruction (pointer) in the basic block's 
	// instruct array.  If realloc fails, exit the loop
	// (terminate the basic block early).

	// Add the next instruction to the basic block's instruct
	// array (at the end), and set that instruction's basic 
	// block (in_block) to point to bb.

	// Set the current instruction to the next instruction, and
	// set the basic block's flags to those of that instruction,
	// which is now the last in the block.

	// Set the next instruction to the current instruction's
	// next_inst field.
    }

    // If the current instruction (the last in the block) has a next
    // instruction, point the first available next_block field to 
    // that instruction's basic block (find it by making a recursive
    // call).  Use n_next, which was initialized to 0, as the array
    // index for next_block, and increment it to indicate that one
    // entry has been used.

    // If the current instruction (the last in the block) has a branch
    // target, point the first available next_block field to 
    // that instruction's basic block (find it by making a recursive
    // call).  Use n_next as the array index for next_block.

    // Return a pointer to the basic block.
    return bb;
}

