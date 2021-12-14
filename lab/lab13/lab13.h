#if !defined(LAB13_H)
#define LAB13_H

typedef struct node_t node_t;

typedef enum {
    NUMBER,	// a number
    OP_ADD,	// an addition
    OP_SUB,	// a subtraction
    OP_MULT,	// a multiplication
    OP_DIV	// a division
} node_type_t;

struct node_t {
    node_type_t type;	// type of node
    double      value;	// value of NUMBER node
    node_t*     left;	// first operand of operator node
    node_t*     right;	// second operand of operator node
};

// Build an expression tree from a string and return a pointer to the
// root of the new tree.  Returns NULL for empty expressions, bad 
// expressions, and other failures.
extern node_t* build_expression (const char* s);

// Print an expression, followed by a line feed.
extern void print_expression (node_t* expr);

// Free an expression and all of its children.
extern void free_expression (node_t* expr);

#endif // LAB13_H

