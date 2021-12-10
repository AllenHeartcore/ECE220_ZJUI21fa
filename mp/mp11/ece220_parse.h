/*									tab:8
 *
 * ece220_parse.h - parser header file for an ECE190 compiler MP (F08 MP5)
 *
 * "Copyright (c) 2008-2018 by Steven S. Lumetta."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written 
 * agreement is hereby granted, provided that the above copyright notice
 * and the following two paragraphs appear in all copies of this software,
 * that the files COPYING and NO_WARRANTY are included verbatim with
 * any distribution, and that the contents of the file README are included
 * verbatim as part of a file named README with any distribution.
 * 
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT, 
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT 
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHOR 
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" 
 * BASIS, AND THE AUTHOR NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Steve Lumetta
 * Version:	    2
 * Creation Date:   15 October 2008
 * Filename:	    ece220_parse.h
 * History:
 *	SL	2	Tue Apr 03 23:17:13 2018
 *		Changed 190 to 220 for ECE220.
 *	SSL	1	15 October 2008
 *		First written.
 */


#if !defined(ECE220_PARSE_H)
#define ECE220_PARSE_H

#include <stdint.h>

typedef enum {
    AST220_PRINTF,
    AST220_RAND,
    AST220_SCANF,
    AST220_SRAND,
    NUM_AST220_BUILTIN_FUNCS
} ast220_builtin_func_t;

typedef enum {

    /*--------------------------------------------------------------------*/
    /* statement types */

    AST220_FOR_STMT,
    AST220_IF_STMT,
    AST220_RETURN_STMT,

    AST220_POP_STACK, /* calculate an expression, discard the result */

    AST220_DEBUG_MARKER, /* add DEBUG(#) to C code to mark positions */

    /*--------------------------------------------------------------------*/
    /* expression types */

    AST220_PUSH_INT,
    AST220_PUSH_STR,
    AST220_VARIABLE, /* read a variable */
    AST220_FUNC_CALL,

    /*
     * These two have AST220_VARIABLE children nodes, but use those nodes
     * differently than do normal expressions (e.g., using the variable's
     * address instead of reading the variable.
     */
    AST220_GET_ADDRESS,
    AST220_OP_ASSIGN,

    /* 
     * increment and decrement: again, these four not only read a 
     * variable, but must also change it
     */
    AST220_OP_PRE_INCR,
    AST220_OP_PRE_DECR,
    AST220_OP_POST_INCR,
    AST220_OP_POST_DECR,

    /* 
     * arithmetic: note that MULTIPLY, DIVIDE, AND MODULUS routines 
     * are provided for you 
     */
    AST220_OP_ADD,
    AST220_OP_SUB,
    AST220_OP_MULT,
    AST220_OP_DIV,
    AST220_OP_MOD,
    AST220_OP_NEGATE,

    /* 
     * logical operators: result should always be 0 or 1; remember to 
     * shortcut AND and OR 
     */
    AST220_OP_LOG_NOT,
    AST220_OP_LOG_OR,
    AST220_OP_LOG_AND,

    /* comparison operators: result should always be 0 or 1 */
    AST220_CMP_NE,
    AST220_CMP_LESS,
    AST220_CMP_LE,
    AST220_CMP_EQ,
    AST220_CMP_GE,
    AST220_CMP_GREATER,

    NUM_AST220_TYPES
} ast220_type_t;


typedef struct ast220_t ast220_t;
struct ast220_t {
    ast220_type_t   type;       /* type of AST node                   */
    int32_t         value;      /* a number (PUSH_INT, DEBUG_MARKER)  */
    char*           name;       /* a string (PUSH_STR, VARIABLE)      */
    ast220_builtin_func_t fnum; /* function number (FUNC_CALL)        */
    ast220_t*       test;       /* test condition (IF_STMT, FOR_STMT) */
    ast220_t*       left;       /* left child/first operand           */
    ast220_t*       middle;     /* middle child (FOR_STMT)            */
    ast220_t*       right;      /* right child/second operand         */
    ast220_t*       next;       /* next AST node                      */
};

extern ast220_t* ece220_parse_program ();
extern int32_t ece220_local_var_space ();
extern int32_t ece220_global_var_space ();

/*
 * AST220_FOR_STMT: test = test condition for loop
 *                  left = initialization expression (or NULL)
 *                  middle = loop body (statements linked by next)
 *                  right = update expression (or NULL)
 * AST220_IF_STMT: test = test condition for if
 *                 left = then clause (statements linked by next)
 *                 right = else clause (statements linked by next)
 * AST220_RETURN_STMT: left = expression to return
 *
 * AST220_POP_STACK: left = expression to produce and then pop (discard)
 *
 * AST220_DEBUG_MARKER: value = marker value
 *
 * AST220_PUSH_INT: value = integer to push onto stack
 * AST220_PUSH_STR: name = string to push onto stack
 * AST220_VARIABLE: name = variable name (look up in symbol table)
 *                  left = array index expression (or NULL)
 *                  NOTE: push to stack if not handled by parent
 *                        (e.g., AS220_GET_ADDRESS or AS220_OP_ASSIGN)
 * AST220_FUNC_CALL: fnum = function id
 *                   left = argument list (rightmost first, linked by next)
 *                   NOTE: leave return value on top of stack (but
 *                         REMOVE arguments from stack!)
 * AST220_GET_ADDRESS: left = AST220_VARIABLE node for which to calculate
 *                            address (NOTE: may have array reference with 
 *                            expression for array index)
 *
 * AST220_OP_ASSIGN: left = AST220_VARIABLE node to which to assign value
 *                   right = expression
 *                   NOTE: copy expression result into variable, but do
 *                         NOT remove it from stack
 * AST220_OP_ADD: left = first operand
 *                right = second operand
 * AST220_OP_SUB: left = first operand
 *                right = second operand
 * AST220_OP_MULT: left = first operand
 *                 right = second operand
 *                 NOTE: use MULTIPLY subroutine in library
 * AST220_OP_DIV: left = first operand
 *                right = second operand
 *                NOTE: use DIVIDE subroutine in library
 * AST220_OP_MOD: left = first operand
 *                right = second operand
 *                NOTE: use MODULUS subroutine in library
 * AST220_OP_NEGATE: left = operand
 *
 * AST220_OP_PRE_INCR: left = AST220_variable to increment
 *                     NOTE: leave incremented result on stack
 * AST220_OP_PRE_DECR: left = AST220_variable to decrement
 *                     NOTE: leave decremented result on stack
 * AST220_OP_POST_INCR: left = AST220_variable to increment
 *                      NOTE: leave original variable value on stack
 * AST220_OP_POST_DECR: left = AST220_variable to decrement
 *                      NOTE: leave original variable value stack
 *
 * AST220_OP_LOG_NOT: left = operand
 * AST220_OP_LOG_OR: left = first operand
 *                   right = second operand
 *                   NOTE: don't forget to shortcut if first operand is TRUE
 * AST220_OP_LOG_AND: left = first operand
 *                    right = second operand
 *                    NOTE: don't forget to shortcut if first operand is FALSE
 *
 * AST220_CMP_NE: left = first operand
 *                right = second operand
 * AST220_CMP_LESS: left = first operand
 *                  right = second operand
 * AST220_CMP_LE: left = first operand
 *                right = second operand
 * AST220_CMP_EQ: left = first operand
 *                right = second operand
 * AST220_CMP_GE: left = first operand
 *                right = second operand
 * AST220_CMP_GREATER: left = first operand
 *                     right = second operand
 */

#endif /* ECE220_PARSE_H */
