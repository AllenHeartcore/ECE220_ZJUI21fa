#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lab13.h"


// Operator precedence levels
typedef enum {
    PREC_LEVEL_NONE,	// no expression seen yet
    PREC_LEVEL_ADD,	// addition / subtraction (same precedence)
    PREC_LEVEL_MULT,	// multiplication / division (same precedence)
} prec_level_t;


// Skip all leading space in a string.  A pointer to a string
// is passed, which is then changed to point to the first 
// non-space character in the string.

static void
skip_space (const char** s)
{
    while (' ' == **s) { (*s)++; }
}


// Returns a new node_t structure filled with bits (or NULL on failure).

static node_t*
create_node (void)
{
    // No variable available, so use type with sizeof.
    return (node_t*)malloc (sizeof (node_t));
}


// Read a number from a string and create a node_t to hold the number.
// A pointer to a string is passed, which is then changed to point
// to the character after the number on success.  If the read fails, 
// or if the allocation fails, NULL is returned, and s is left
// unmodified.

static node_t*
read_number (const char** s)
{
    char*   rval; // Return value cannot overlap with value passed.
    node_t* n = create_node ();

    if (NULL != n) {
        n->type = NUMBER;
	// Must check for errors in strtod using errno.
	errno = 0;
	n->value = strtod (*s, &rval);
	if (0 != errno) {
	    free (n);
	    return NULL;
	}
	// Copy return value into *s to return it to caller on success.
	*s = rval;
    }
    return n;
}


// Free an expression and all of its children.

void
free_expression (node_t* expr)
{
    if (NUMBER != expr->type) {
	// Anything other than a number has two operands.
        free_expression (expr->left);
        free_expression (expr->right);
    }
    // Free the node_t itself last.
    free (expr);
}


// Make a new operator node of type op with two operands as given.
// If allocation fails, or if either operand is NULL, any non-NULL
// operand is freed along with the node_t just allocated (if any),
// and NULL is returned.

static node_t*
make_operator (node_type_t op, node_t* first, node_t* second)
{
    node_t* n = create_node ();

    if (NULL != n) {
        n->type = op;
	n->left = first;
	n->right = second;
    }

    if (NULL == n || NULL == first || NULL == second) {
        if (NULL != first) {
	    free_expression (first);
	}
        if (NULL != second) {
	    free_expression (second);
	}
	free (n);
	printf ("empty operand\n");
	return NULL;
    }

    return n;
}


// This call is hidden from outside code; parenthesis level and 
// precedence are not known other than in this file.

static node_t*
do_build_expression (const char** s, uint32_t paren_level, 
		     prec_level_t prec_level)
{
    node_t*     expr; 		// current expression
    node_type_t op;		// operation found
    prec_level_t new_prec;	// precedence level of op

    // Ignore leading spaces.
    skip_space (s);

    // Empty expressions return NULL.
    if ('\0' == **s || '\n' == **s) {
        return NULL;
    }

    // Expression starts with a parenthesized expression or a number.
    if ('(' == **s) {
	// Skip the open parenthesis.
	(*s)++;
	// Build the expression between parentheses.
        expr = do_build_expression (s, paren_level + 1, PREC_LEVEL_NONE);
    } else if (NULL == (expr = read_number (s))) {
	// Trying to read a number failed.
	printf ("bad character '%c' at start of expression\n", **s);
	return NULL;
    }

    // Continue to parse until out of input.
    while (1) {

	// All done with input?  Return expr.
	skip_space (s);
	if ('\0' == **s || '\n' == **s) {
	    return expr;
	}

	// Otherwise, must be followed by an operator / close parenthesis.
	switch (**s) {
	    case '+': op = OP_ADD;  new_prec = PREC_LEVEL_ADD;  break;
	    case '-': op = OP_SUB;  new_prec = PREC_LEVEL_ADD;  break;
	    case '*': op = OP_MULT; new_prec = PREC_LEVEL_MULT; break;
	    case '/': op = OP_DIV;  new_prec = PREC_LEVEL_MULT; break;
	    case ')':
		if (0 < paren_level) {
		    (*s)++;
		    return expr;
		}
		// Otherwise, fall through--cannot close parentheses
		// without opening them.
	    default:
		printf ("bad operator '%c'\n", **s);
		free_expression (expr);
		return NULL;
	}

	// Check whether previous operator has equal or higher precedence:
	// if so, do it first by returning current expression.  
	// So A + B + C means (A + B) + C.
	if (new_prec <= prec_level) {
	    return expr;
	}

	// Skip the operator.
	(*s)++;

	// Make an operator node.
	expr =  make_operator (op, expr, do_build_expression 
			       (s, paren_level, new_prec));

	if (NULL == expr) {
	    // Operator node creation failed.  Return NULL.
	    return NULL;
	}
    }
}


// Build an expression tree from a string and return a pointer to the
// root of the new tree.  Returns NULL for empty expressions, bad 
// expressions, and other failures.

node_t*
build_expression (const char* s)
{
    const char* ptr = s;
    return do_build_expression (&ptr, 0, PREC_LEVEL_NONE);
}


// Print the expression rooted at expr.

static void
do_print_expression (node_t* expr)
{
    switch (expr->type) {
        case NUMBER:
	    printf ("%f", expr->value);
	    break;
	case OP_ADD:
	    printf ("(");
	    do_print_expression (expr->left);
	    printf (" + ");
	    do_print_expression (expr->right);
	    printf (")");
	    break;
	case OP_SUB:
	    printf ("(");
	    do_print_expression (expr->left);
	    printf (" - ");
	    do_print_expression (expr->right);
	    printf (")");
	    break;
	case OP_MULT:
	    printf ("(");
	    do_print_expression (expr->left);
	    printf (" * ");
	    do_print_expression (expr->right);
	    printf (")");
	    break;
	case OP_DIV:
	    printf ("(");
	    do_print_expression (expr->left);
	    printf (" / ");
	    do_print_expression (expr->right);
	    printf (")");
	    break;
    	default:
	    break;
    }
}


// Print an expression, followed by a line feed.

void
print_expression (node_t* expr)
{
    do_print_expression (expr);
    puts ("");
}


