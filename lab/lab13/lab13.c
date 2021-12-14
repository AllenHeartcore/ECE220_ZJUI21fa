#include <stdint.h>
#include <stdio.h>

#include "lab13.h"


// Calculate and return the value of an expression recursively.
// The root of the expression is the node expr.  See the header file
// for the node definition.

static double 
evaluate_expression (node_t* expr)
{
    return 0;
}


int 
main ()
{
    char    buf[200];	// buffer for a line
    node_t* expr;	// an expression
    double  result;	// the value of the expression

    // Read a line from stdin into the buffer buf.  Continue the loop
    // until the call fails.  (Press CTRL-D at the start of a line to
    // terminate the program, once you've written the I/O call.
    while (0) { // fix this expression
        expr = build_expression (buf);
	if (NULL != expr) {
	    print_expression (expr);
	    result = evaluate_expression (expr);
	    printf ("result is %f\n", result);
	    free_expression (expr);
	}
    }

    return 0;
}

