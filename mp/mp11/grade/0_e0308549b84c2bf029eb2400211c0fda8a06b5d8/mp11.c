/*	###### Copyright Information ######
	This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem XI. 
		Written			Commented		Debugged		Commited
		17, 30 Dec 2021		31 Dec 2021		30, 31 Dec 2021		31 Dec 2021
	The following 11 functions are written by Steve Lumetta. 
		gen_long_branch		gen_statement		gen_debug_marker	gen_expression
		gen_comparison		gen_op_cmp_ne		gen_op_cmp_less		gen_op_cmp_le
		gen_op_cmp_eq		gen_op_cmp_ge		gen_op_cmp_greater
	Copyright © 2021 Lumetta, Steve and Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	cat tests/${PROGRAM_NAME}.c | ./c220 > ${PROGRAM_NAME}.asm
	The program outputs:	A .asm code file named "${PROGRAM_NAME}.asm"

	###### Function Documentation ######
	Refer to respective function headers. 
 */

/*									tab:8
 *
 * mp11.c - skeleton code generation for ECE190 compiler MP (F08 MP5)
 *
 * "Copyright (c) 2008-2018 by Steven S. Lumetta."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Steve Lumetta
 * Version:	    3
 * Creation Date:   Wed Oct 15 10:16:14 2008
 * Filename:	    mp11.c
 * History:
 *	SL	3	Tue Apr 03 23:23:23 2018
 *		Changed MP5 to MP11 and 190 to 220 for ECE220.
 *	SL	2	Tue Oct 21 14:28:10 2008
 *		Extracted from gold version.
 *	SL	1	Wed Oct 15 10:16:14 2008
 *		First written.
 */


#include <stdio.h>

#include "ece220_label.h"
#include "ece220_parse.h"
#include "ece220_symtab.h"


/*
 * PARAMETERS AND CONSTANTS
 */

typedef enum {
    BR_NEVER, BR_P, BR_Z, BR_ZP, BR_N, BR_NP, BR_NZ, BR_ALWAYS, NUM_BR
} br_type_t;



/*
 * HELPER FUNCTION PROTOTYPES--see function headers for further information
 */

static void gen_long_branch (br_type_t type, ece220_label_t* label);

static void gen_statement (ast220_t* ast);
static void gen_for_statement (ast220_t* ast);
static void gen_if_statement (ast220_t* ast);
static void gen_return_statement (ast220_t* ast);
static void gen_pop_stack (ast220_t* ast);
static void gen_debug_marker (ast220_t* ast);

static void gen_expression (ast220_t* ast);
static void gen_push_int (ast220_t* ast);
static void gen_push_str (ast220_t* ast);
static void gen_push_variable (ast220_t* ast);
static void gen_func_call (ast220_t* ast);
static void gen_get_address (ast220_t* ast);
static void gen_op_assign (ast220_t* ast);
static void gen_op_pre_incr (ast220_t* ast);
static void gen_op_pre_decr (ast220_t* ast);
static void gen_op_post_incr (ast220_t* ast);
static void gen_op_post_decr (ast220_t* ast);
static void gen_op_add (ast220_t* ast);
static void gen_op_sub (ast220_t* ast);
static void gen_op_mult (ast220_t* ast);
static void gen_op_div (ast220_t* ast);
static void gen_op_mod (ast220_t* ast);
static void gen_op_negate (ast220_t* ast);
static void gen_op_log_not (ast220_t* ast);
static void gen_op_log_or (ast220_t* ast);
static void gen_op_log_and (ast220_t* ast);
static void gen_comparison (ast220_t* ast, const char* false_cond);
static void gen_op_cmp_ne (ast220_t* ast);
static void gen_op_cmp_less (ast220_t* ast);
static void gen_op_cmp_le (ast220_t* ast);
static void gen_op_cmp_eq (ast220_t* ast);
static void gen_op_cmp_ge (ast220_t* ast);
static void gen_op_cmp_greater (ast220_t* ast);


/*
 * FILE SCOPE VARIABLES
 * 
 * N.B.  You will need to have a file scope variable to implement one
 * of the statements.  Define it here.
 *
 * static int foo;  <-- a file scope variable ("static" means only this file)
 *
 */
static ece220_label_t* return_label = NULL;


/* 
 * INTERFACE FUNCTIONS -- called from other files; the one function listed
 * here is the equivalent of "main" for students working on this MP
 */

/* 
 * MP11_generate_code
 *   DESCRIPTION: perform LC-3 code generation (print to stdout)
 *   INPUTS: prog -- the main subroutine of the program (a list of
 *                   statements)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */

void MP11_generate_code (ast220_t* prog) {
	if (!return_label) return_label = label_create();		// Init. return_label in the 1st entry
	switch (prog->type) {						// Distributor
		case AST220_FOR_STMT:
		case AST220_IF_STMT:
		case AST220_RETURN_STMT:
		case AST220_POP_STACK:
		case AST220_DEBUG_MARKER: gen_statement(prog); break;	// 5 kinds of statements
		default: gen_expression(prog); break;			// 25 kinds of expressions
	}
	if (prog->next) MP11_generate_code(prog->next);			// Generate code for the next node
	else printf("%s\n", label_value(return_label));			// Print final return label
}

/*
 * HELPER FUNCTIONS (used only within this file)
 */

/* 
 * gen_long_branch
 *   DESCRIPTION: generate LC-3 assembly code for a branch without offset
 *                limitations
 *   INPUTS: type -- the type of branch to be performed
 *           label -- the branch target label
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_long_branch (br_type_t type, ece220_label_t* label)
{
    static const char* const br_names[NUM_BR] = {
        "; ", "BRp", "BRz", "BRzp", "BRn", "BRnp", "BRnz", "BRnzp"
    }; 
    br_type_t neg_type;
    ece220_label_t* target_label;
    ece220_label_t* false_label;

    neg_type = (type ^ 7);
    target_label = label_create ();
    false_label = label_create ();
    printf ("\t%s %s\n", br_names[neg_type], label_value (false_label));
    printf ("\tLD R3,%s\n\tJMP R3\n", label_value (target_label));
    printf ("%s\n", label_value (target_label));
    printf ("\t.FILL %s\n", label_value (label));
    printf ("%s\n", label_value (false_label));
}

/* 
 * gen_statement
 *   DESCRIPTION: generate LC-3 assembly code for an arbitrary statement
 *   INPUTS: ast -- the AST node corresponding to the statement
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_statement (ast220_t* ast)
{
    switch (ast->type) {
	case AST220_FOR_STMT:     gen_for_statement (ast);    break;
	case AST220_IF_STMT:      gen_if_statement (ast);     break;
	case AST220_RETURN_STMT:  gen_return_statement (ast); break;
	case AST220_POP_STACK:    gen_pop_stack (ast);        break;
	case AST220_DEBUG_MARKER: gen_debug_marker (ast);     break;
	default: fputs ("BAD STATEMENT TYPE\n", stderr);      break;
    }
}

/* 
 * gen_for_statement
 *   DESCRIPTION: generate LC-3 assembly code for an for loop
 *   INPUTS: ast -- the AST node corresponding to the for loop
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_for_statement (ast220_t* ast) {
	ece220_label_t* loop_label = label_create();
	ece220_label_t* break_label = label_create();
	if (ast->left) gen_statement(ast->left);			// Loop init. (may be NULL)
	printf("%s\n", label_value(loop_label));
	gen_expression(ast->test);					// Loop test
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n");
	gen_long_branch(2, break_label);				// Break out of loop
	ast220_t* current_stmt = ast->middle;
	while (current_stmt) {
		gen_statement(current_stmt);				// Loop body
		current_stmt = current_stmt->next;
	}
	if (ast->right) gen_statement(ast->right);			// Loop update
	gen_long_branch(7, loop_label);					// Break back to loop test
	printf("%s\n", label_value(break_label));
}

/* 
 * gen_if_statement
 *   DESCRIPTION: generate LC-3 assembly code for an if statement
 *   INPUTS: ast -- the AST node corresponding to the if statement
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_if_statement (ast220_t* ast) {
	ece220_label_t* false_label = label_create();
	ece220_label_t* cont_label;
	gen_expression(ast->test);					// Test
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n");
	gen_long_branch(2, false_label);				// Break to false
	ast220_t* current_stmt = ast->left;
	while (current_stmt) {
		gen_statement(current_stmt);				// Tested true
		current_stmt = current_stmt->next;
	}
	if (ast->right) {
		cont_label = label_create();
		gen_long_branch(7, cont_label);				// Break to continue
	}
	printf("%s\n", label_value(false_label));
	if (ast->right) {
		current_stmt = ast->right;
		while (current_stmt) {
			gen_statement(current_stmt);			// Tested false
			current_stmt = current_stmt->next;
		}
		printf("%s\n", label_value(cont_label));
	}
}

/* 
 * gen_return_statement
 *   DESCRIPTION: generate LC-3 assembly code for a return statement
 *   INPUTS: ast -- the AST node corresponding to the return statement
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_return_statement (ast220_t* ast) {
	gen_expression(ast->left);					// Calc. return value
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tSTR R0,R5,#3\n");
	gen_long_branch(7, return_label);				// Break to end of program
}

/* 
 * gen_pop_stack
 *   DESCRIPTION: generate LC-3 assembly code to pop and discard a value
 *                from the stack
 *   INPUTS: ast -- the AST node corresponding to the pop stack statement
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_pop_stack (ast220_t* ast) {
	gen_expression(ast->left);
	printf("\tADD R6,R6,#1\n");
}

/* 
 * gen_debug_marker
 *   DESCRIPTION: generate LC-3 assembly code for a debug marker used to
 *                help debug LC-3 code generation; the marker is simply
 *                a comment in the assembly code that can be used to
 *                identify pieces of code generated by the compiler
 *   INPUTS: ast -- the AST node corresponding to the debug marker
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints an LC-3 assembly comment
 */
static void 
gen_debug_marker (ast220_t* ast)
{
    printf ("; --------------- DEBUG(%d) ---------------\n", ast->value);
}

/* 
 * gen_expression
 *   DESCRIPTION: generate LC-3 assembly code to calculate an arbitrary
 *                expression and push the result onto the stack
 *   INPUTS: ast -- the AST node corresponding to the expression
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_expression (ast220_t* ast)
{
    switch (ast->type) {
	case AST220_PUSH_INT:     gen_push_int (ast);       break;
	case AST220_PUSH_STR:     gen_push_str (ast);       break;
	case AST220_VARIABLE:     gen_push_variable (ast);  break;
	case AST220_FUNC_CALL:    gen_func_call (ast);      break;
	case AST220_GET_ADDRESS:  gen_get_address (ast);    break;
	case AST220_OP_ASSIGN:    gen_op_assign (ast);      break;
	case AST220_OP_PRE_INCR:  gen_op_pre_incr (ast);    break;
	case AST220_OP_PRE_DECR:  gen_op_pre_decr (ast);    break;
	case AST220_OP_POST_INCR: gen_op_post_incr (ast);   break;
	case AST220_OP_POST_DECR: gen_op_post_decr (ast);   break;
	case AST220_OP_ADD:       gen_op_add (ast);         break;
	case AST220_OP_SUB:       gen_op_sub (ast);         break;
	case AST220_OP_MULT:      gen_op_mult (ast);        break;
	case AST220_OP_DIV:       gen_op_div (ast);         break;
	case AST220_OP_MOD:       gen_op_mod (ast);         break;
	case AST220_OP_NEGATE:    gen_op_negate (ast);      break;
	case AST220_OP_LOG_NOT:   gen_op_log_not (ast);     break;
	case AST220_OP_LOG_OR:    gen_op_log_or (ast);      break;
	case AST220_OP_LOG_AND:   gen_op_log_and (ast);     break;
	case AST220_CMP_NE:       gen_op_cmp_ne (ast);      break;
	case AST220_CMP_LESS:     gen_op_cmp_less (ast);    break;
	case AST220_CMP_LE:       gen_op_cmp_le (ast);      break;
	case AST220_CMP_EQ:       gen_op_cmp_eq (ast);      break;
	case AST220_CMP_GE:       gen_op_cmp_ge (ast);      break;
	case AST220_CMP_GREATER:  gen_op_cmp_greater (ast); break;
	default: fputs ("BAD EXPRESSION TYPE\n", stderr);   break;
    }
}

/* 
 * gen_push_int
 *   DESCRIPTION: generate LC-3 assembly code to push a constant integer
 *                expression onto the stack
 *   INPUTS: ast -- the AST node corresponding to the integer
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_push_int (ast220_t* ast) {
	ece220_label_t* value_label = label_create();
	ece220_label_t* instr_label = label_create();
	printf("\tLD R0,%s\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n\tBRnzp %s\n%s\n\t.FILL #%d\n%s\n", 	// R0 - Value
		label_value(value_label), label_value(instr_label), label_value(value_label), ast->value, label_value(instr_label));
}

/* 
 * gen_push_str
 *   DESCRIPTION: generate LC-3 assembly code to push a constant string
 *                expression onto the stack
 *   INPUTS: ast -- the AST node corresponding to the string
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_push_str (ast220_t* ast) {
	ece220_label_t* value_label = label_create();
	ece220_label_t* instr_label = label_create();
	printf("\tLEA R0,%s\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n\tBRnzp %s\n%s\n\t.STRINGZ %s\n%s\n", 	// R0 - Addr. of string
		label_value(value_label), label_value(instr_label), label_value(value_label), ast->name, label_value(instr_label));
}

/* 
 * gen_push_variable
 *   DESCRIPTION: generate LC-3 assembly code to push a variable's value
 *                onto the stack
 *   INPUTS: ast -- the AST node corresponding to the variable reference
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_push_variable (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->name);					// Lookup variable
	if (!ast->left) {
		printf("\tLDR R0,R%d,#%d\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 				// R0 - Value of variable
			5 - variable_entry->is_global, variable_entry->offset);
	} else {
		gen_expression(ast->left);								// R1 - Array offset -> Addr. of variable
		printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R%d,#%d\n\tADD R1,R1,R0\n\tLDR R0,R1,#0\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 
			5 - variable_entry->is_global, variable_entry->offset);
	}
}

/* 
 * gen_func_call
 *   DESCRIPTION: generate LC-3 assembly code to perform a function call,
 *                remove the arguments from the stack, and leave the
 *                return value on top of the stack
 *   INPUTS: ast -- the AST node corresponding to the function call
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_func_call (ast220_t* ast) {
	ast220_t* args = ast->left;
	int32_t args_count = 0;
	while (args) {
		gen_expression(args);									// Calc. arguments
		args = args->next;
		args_count++;
	}
	ece220_label_t* subrt_label = label_create();
	ece220_label_t* instr_label = label_create();
	printf("\tLD R0,%s\n\tJSRR R0\n\tBRnzp %s\n%s\n\t.FILL ", 					// R0 - Addr. of target function
		label_value(subrt_label), label_value(instr_label), label_value(subrt_label));
	switch (ast->fnum) {
		case AST220_PRINTF:	printf("PRINTF\n");	break;
		case AST220_RAND:	printf("RAND\n");	break;
		case AST220_SCANF:	printf("SCANF\n");	break;
		case AST220_SRAND:	printf("SRAND\n");	break;
		default: break;
	}
	printf("%s\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R6,R6,#%d\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", // R0 - Return value of function
		label_value(instr_label), args_count);
}

/* 
 * gen_get_address
 *   DESCRIPTION: generate LC-3 assembly code to push the address of
 *                a variable onto the stack
 *   INPUTS: ast -- the AST node corresponding to the variable reference
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_get_address (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->left->name);				// Lookup variable
	if (!ast->left->left) {
		printf("\tADD R0,R%d,#%d\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 				// R0 - Addr. of variable
			5 - variable_entry->is_global, variable_entry->offset);
	} else {
		gen_expression(ast->left->left);							// R1 - Array offset
		printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R%d,#%d\n\tADD R0,R1,R0\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 
			5 - variable_entry->is_global, variable_entry->offset);
	}
}

/* 
 * gen_op_assign
 *   DESCRIPTION: generate LC-3 assembly code to perform an assignment,
 *                leaving the value assigned on the stack (assignments
 *                are also expressions, with value equal to that of the
 *                value assigned)
 *   INPUTS: ast -- the AST node corresponding to the assignment
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_assign (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->left->name);				// Lookup variable
	gen_expression(ast->right);									// R1 - Assigned value
	if (!ast->left->left) {										// Scalar case
		printf("\tADD R0,R%d,#%d\n\tLDR R1,R6,#0\n\tSTR R1,R0,#0\n", 				// R0 - Addr. of variable
			5 - variable_entry->is_global, variable_entry->offset);
	} else {											// Array case
		gen_expression(ast->left->left);							// R1 - Array offset
		printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R%d,#%d\n\tADD R0,R1,R0\n\tLDR R1,R6,#0\n\tSTR R1,R0,#0\n", 
			5 - variable_entry->is_global, variable_entry->offset);
	}
}

/* 
 * gen_op_pre_incr
 *   DESCRIPTION: generate LC-3 assembly code to for a pre-increment 
 *                expression, which increments a variable and pushes
 *                the new value of the variable onto the stack
 *   INPUTS: ast -- the AST node corresponding to pre-increment
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_pre_incr (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->left->name);				// Lookup variable
	printf("\tADD R0,R%d,#%d\n\tLDR R1,R0,#0\n\tADD R1,R1,#1\n\tSTR R1,R0,#0\n\tADD R6,R6,#-1\n\tSTR R1,R6,#0\n", 
		5 - variable_entry->is_global, variable_entry->offset);					// R0 - Addr., R1 - Value
}

/* 
 * gen_op_pre_decr
 *   DESCRIPTION: generate LC-3 assembly code to for a pre-decrement 
 *                expression, which decrements a variable and pushes
 *                the new value of the variable onto the stack
 *   INPUTS: ast -- the AST node corresponding to pre-decrement
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_pre_decr (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->left->name);				// ^
	printf("\tADD R0,R%d,#%d\n\tLDR R1,R0,#0\n\tADD R1,R1,#-1\n\tSTR R1,R0,#0\n\tADD R6,R6,#-1\n\tSTR R1,R6,#0\n", 
		5 - variable_entry->is_global, variable_entry->offset);
}

/* 
 * gen_op_post_incr
 *   DESCRIPTION: generate LC-3 assembly code to for a post-increment 
 *                expression, which increments a variable and pushes
 *                the original value of the variable onto the stack
 *   INPUTS: ast -- the AST node corresponding to post-increment
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_post_incr (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->left->name);				// ^^
	printf("\tADD R0,R%d,#%d\n\tLDR R1,R0,#0\n\tADD R2,R1,#1\n\tSTR R2,R0,#0\n\tADD R6,R6,#-1\n\tSTR R1,R6,#0\n", 
		5 - variable_entry->is_global, variable_entry->offset);
}

/* 
 * gen_op_post_decr
 *   DESCRIPTION: generate LC-3 assembly code to for a post-decrement 
 *                expression, which decrements a variable and pushes
 *                the original value of the variable onto the stack
 *   INPUTS: ast -- the AST node corresponding to post-decrement
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_post_decr (ast220_t* ast) {
	symtab_entry_t* variable_entry = symtab_lookup(ast->left->name);				// ^^^
	printf("\tADD R0,R%d,#%d\n\tLDR R1,R0,#0\n\tADD R2,R1,#-1\n\tSTR R2,R0,#0\n\tADD R6,R6,#-1\n\tSTR R1,R6,#0\n", 
		5 - variable_entry->is_global, variable_entry->offset);
}

/* 
 * gen_op_add
 *   DESCRIPTION: generate LC-3 assembly code to calculate the sum of two
 *                expressions, leaving the sum on the stack
 *   INPUTS: ast -- the AST node corresponding to the add operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_add (ast220_t* ast) {
	gen_expression(ast->left);									// Calc. arg 1
	gen_expression(ast->right);									// Calc. arg 2
	printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n");			// R0 - Arg 1, R1 - Arg 2
	printf("\tADD R0,R0,R1\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n");
}

/* 
 * gen_op_sub
 *   DESCRIPTION: generate LC-3 assembly code to calculate the difference 
 *                of two expressions, leaving the difference on the stack
 *   INPUTS: ast -- the AST node corresponding to the subtraction operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_sub (ast220_t* ast) {
	gen_expression(ast->left);									// ^
	gen_expression(ast->right);
	printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n");
	printf("\tNOT R1,R1\n\tADD R1,R1,#1\n\tADD R0,R0,R1\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n");
}

/* 
 * gen_op_mult
 *   DESCRIPTION: generate LC-3 assembly code to calculate the product 
 *                of two expressions (using the MULTIPLY subroutine in
 *                the LC-3 library), leaving the product on the stack
 *   INPUTS: ast -- the AST node corresponding to the multiplication operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_mult (ast220_t* ast) {
	gen_expression(ast->left);									// ^^
	gen_expression(ast->right);
	ece220_label_t* subroutine_label = label_create();
	ece220_label_t* instr_label = label_create();
	printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n");			// R3 - Addr. of target function
	printf("\tLD R3,%s\n\tJSRR R3\n\tBRnzp %s\n%s\n\t.FILL MULTIPLY\n%s\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 
		label_value(subroutine_label), label_value(instr_label), label_value(subroutine_label), label_value(instr_label));
}

/* 
 * gen_op_div
 *   DESCRIPTION: generate LC-3 assembly code to calculate the quotient 
 *                of two expressions (using the DIVIDE subroutine in
 *                the LC-3 library), leaving the quotient on the stack
 *   INPUTS: ast -- the AST node corresponding to the division operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_div (ast220_t* ast) {
	gen_expression(ast->left);									// ^^^
	gen_expression(ast->right);
	ece220_label_t* subroutine_label = label_create();
	ece220_label_t* instr_label = label_create();
	printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n");
	printf("\tLD R3,%s\n\tJSRR R3\n\tBRnzp %s\n%s\n\t.FILL DIVIDE\n%s\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 
		label_value(subroutine_label), label_value(instr_label), label_value(subroutine_label), label_value(instr_label));
}

/* 
 * gen_op_mod
 *   DESCRIPTION: generate LC-3 assembly code to calculate the modulus 
 *                of one expression relative to a second expression (using 
 *                the MODULUS subroutine in the LC-3 library), leaving 
 *                the result on the stack
 *   INPUTS: ast -- the AST node corresponding to the division operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_mod (ast220_t* ast) {
	gen_expression(ast->left);									// ^^^^
	gen_expression(ast->right);
	ece220_label_t* subroutine_label = label_create();
	ece220_label_t* instr_label = label_create();
	printf("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n");
	printf("\tLD R3,%s\n\tJSRR R3\n\tBRnzp %s\n%s\n\t.FILL MODULUS\n%s\n\tADD R6,R6,#-1\n\tSTR R0,R6,#0\n", 
		label_value(subroutine_label), label_value(instr_label), label_value(subroutine_label), label_value(instr_label));
}

/* 
 * gen_op_negate
 *   DESCRIPTION: generate LC-3 assembly code to negate the value of an
 *                expression, leaving the result on the stack
 *   INPUTS: ast -- the AST node corresponding to the negation operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_negate (ast220_t* ast) {
	gen_expression(ast->left);									// Calc. arg
	printf("\tLDR R0,R6,#0\n\tNOT R0,R0\n\tADD R0,R0,#1\n\tSTR R0,R6,#0\n");			// R0 - arg -> -arg
}

/* 
 * gen_op_log_not
 *   DESCRIPTION: generate LC-3 assembly code to perform a logical NOT
 *                operation on an expression, leaving the result (0 or 1)
 *                on the stack
 *   INPUTS: ast -- the AST node corresponding to the logical NOT operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_log_not (ast220_t* ast) {
	ece220_label_t* branch_label = label_create();
	gen_expression(ast->left);									// Calc. arg
	printf("\tAND R2,R2,#0\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n\tBRnp %s\n\tADD R2,R2,#1\n", 
		label_value(branch_label));								// R2 - Result
	printf("%s\n\tADD R6,R6,#-1\n\tSTR R2,R6,#0\n", 
		label_value(branch_label));
}

/* 
 * gen_op_log_or
 *   DESCRIPTION: generate LC-3 assembly code to perform a logical OR
 *                operation on two expressions, leaving the result (0 or 1)
 *                on the stack; the code generated must perform proper
 *                shortcutting, i.e., if the first expression is true
 *                the second is not evaluated (the code is not executed)
 *   INPUTS: ast -- the AST node corresponding to the logical OR operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_log_or (ast220_t* ast) {
	ece220_label_t* cmptf_label = label_create();
	ece220_label_t* cmptt_label = label_create();
	gen_expression(ast->left);									// Calc. arg 1
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n");
	gen_long_branch(5, cmptf_label);								// Shortcut when arg 1 is true
	gen_expression(ast->right);									// Calc. arg 2
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n\tBRnp %s\n\tAND R2,R2,#0\n\tBRnzp %s\n", 
		label_value(cmptf_label), label_value(cmptt_label));					// R2 - Result
	printf("%s\n\tAND R2,R2,#0\n\tADD R2,R2,#1\n%s\n\tADD R6,R6,#-1\n\tSTR R2,R6,#0\n", 
		label_value(cmptf_label), label_value(cmptt_label));
}

/* 
 * gen_op_log_and
 *   DESCRIPTION: generate LC-3 assembly code to perform a logical AND
 *                operation on two expressions, leaving the result (0 or 1)
 *                on the stack; the code generated must perform proper
 *                shortcutting, i.e., if the first expression is false
 *                the second is not evaluated (the code is not executed)
 *   INPUTS: ast -- the AST node corresponding to the logical AND operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void gen_op_log_and (ast220_t* ast) {
	ece220_label_t* cmptf_label = label_create();
	ece220_label_t* cmptt_label = label_create();
	gen_expression(ast->left);									// ^
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n");
	gen_long_branch(2, cmptf_label);								// Shortcut when arg 1 is false
	gen_expression(ast->right);
	printf("\tLDR R0,R6,#0\n\tADD R6,R6,#1\n\tADD R0,R0,#0\n\tBRz %s\n\tAND R2,R2,#0\n\tADD R2,R2,#1\n\tBRnzp %s\n", 
		label_value(cmptf_label), label_value(cmptt_label));
	printf("%s\n\tAND R2,R2,#0\n%s\n\tADD R6,R6,#-1\n\tSTR R2,R6,#0\n", 
		label_value(cmptf_label), label_value(cmptt_label));
}

/* 
 * gen_comparison
 *   DESCRIPTION: generate LC-3 assembly code for a comparison operator,
 *                leaving the result (0 or 1) on the stack
 *   INPUTS: ast -- the AST node corresponding to the comparison
 *           false_cond -- a string representation of the condition codes 
 *                         for which the comparison is false (as calculated
 *                         by subtracting the second expression from the 
 *                         first)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 *   KNOWN BUGS: ignores 2's complement overflow in making the comparison
 */
static void
gen_comparison (ast220_t* ast, const char* false_cond)
{
    ece220_label_t* false_label;

    false_label = label_create ();
    gen_expression (ast->left);
    gen_expression (ast->right);
    printf ("\tLDR R1,R6,#0\n\tADD R6,R6,#1\n\tLDR R0,R6,#0\n\tADD R6,R6,#1\n");
    printf ("\tAND R2,R2,#0\n\tNOT R1,R1\n\tADD R1,R1,#1\n\tADD R0,R0,R1\n");
    printf ("\tBR%s %s\n\tADD R2,R2,#1\n", false_cond,
	    label_value (false_label));
    printf ("%s\n", label_value (false_label));
    printf ("\tADD R6,R6,#-1\n\tSTR R2,R6,#0\n");
}

/* 
 * gen_op_cmp_ne
 *   DESCRIPTION: generate LC-3 assembly code to compare whether the value
 *                of one expression is not equal to the value of a second 
 *                expression, leaving the result (0 or 1) on the stack
 *   INPUTS: ast -- the AST node corresponding to the inequality operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_op_cmp_ne (ast220_t* ast)
{
    gen_comparison (ast, "z");
}

/* 
 * gen_op_cmp_less
 *   DESCRIPTION: generate LC-3 assembly code to compare whether the value
 *                of one expression is less than the value of a second
 *                expression, leaving the result (0 or 1) on the stack
 *   INPUTS: ast -- the AST node corresponding to the less-than operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_op_cmp_less (ast220_t* ast)
{
    gen_comparison (ast, "zp");
}

/* 
 * gen_op_cmp_le
 *   DESCRIPTION: generate LC-3 assembly code to compare whether the value
 *                of one expression is less than or equal to the value 
 *                of a second expression, leaving the result (0 or 1) on 
 *                the stack
 *   INPUTS: ast -- the AST node corresponding to the less-or-equal-to 
 *                  operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_op_cmp_le (ast220_t* ast)
{
    gen_comparison (ast, "p");
}

/* 
 * gen_op_cmp_eq
 *   DESCRIPTION: generate LC-3 assembly code to compare whether the value
 *                of one expression is equal to the value of a second 
 *                expression, leaving the result (0 or 1) on the stack
 *   INPUTS: ast -- the AST node corresponding to the equality operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_op_cmp_eq (ast220_t* ast)
{
    gen_comparison (ast, "np");
}

/* 
 * gen_op_cmp_ge
 *   DESCRIPTION: generate LC-3 assembly code to compare whether the value
 *                of one expression is greater than or equal to the value 
 *                of a second expression, leaving the result (0 or 1) on 
 *                the stack
 *   INPUTS: ast -- the AST node corresponding to the greater-or-equal-to 
 *                  operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_op_cmp_ge (ast220_t* ast)
{
    gen_comparison (ast, "n");
}

/* 
 * gen_op_cmp_greater
 *   DESCRIPTION: generate LC-3 assembly code to compare whether the value
 *                of one expression is greater than the value of a second
 *                expression, leaving the result (0 or 1) on the stack
 *   INPUTS: ast -- the AST node corresponding to the greater-than operation
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints LC-3 instructions
 */
static void 
gen_op_cmp_greater (ast220_t* ast)
{
    gen_comparison (ast, "nz");
}

