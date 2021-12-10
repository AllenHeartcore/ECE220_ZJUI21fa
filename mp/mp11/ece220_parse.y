/*									tab:8
 *
 * ece220_parse.y - parser definition for an ECE190 compiler MP (F08 MP5)
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
 * Filename:	    ece220_parse.y
 * History:
 *	SL	2	Tue Apr 03 23:21:03 2018
 *		Changed 190 to 220 for ECE220.
 *	SSL	1	15 October 2008
 *		First written.
 */

%{
    #include <stdio.h>
    #include <stdlib.h>

    #include "ece220_lex.h"
    #include "ece220_parse.h"
    #include "ece220_symtab.h"
    #define YYSTYPE ast220_t*

    extern int ece220lex ();

    void parse_program ();

    static void var_create (ast220_t* name, ast220_t* a_len);
    static void start_main ();
    static ast220_t* new_ast_node ();
    static ast220_t* new_ast_num (int32_t value);
    static ast220_t* new_ast_str (char* str);
    static ast220_t* new_ast_func (ast220_builtin_func_t id);
    static ast220_t* new_ast_op (ast220_type_t type, ast220_t* left, 
    				 ast220_t* right);

    static int32_t reading_globals;
    static int32_t global_offset;
    static int32_t local_offset;
    static int32_t has_main;

    static ast220_t* ece220_main;
%}

%token C220_KEY_IDENT 
%token C220_KEY_INT C220_KEY_MAIN
%token C220_KEY_INT_TYPE C220_KEY_STRING
%token C220_KEY_ELSE C220_KEY_FOR C220_KEY_IF C220_KEY_RETURN
%token C220_KEY_PRINTF C220_KEY_RAND C220_KEY_SCANF C220_KEY_SRAND
%token C220_KEY_INCR C220_KEY_DECR C220_KEY_UNARY_NEG
%token C220_KEY_DEBUG_MARKER

%right '='
%left  C220_KEY_AND C220_KEY_OR
%left  C220_KEY_EQ C220_KEY_NE
%left  '<' C220_KEY_LE C220_KEY_GE '>'
%left  '+' '-'
%left  '*' '/' '%'
%right '&' C220_KEY_UNARY_NEG '!'
%right C220_KEY_INCR C220_KEY_DECR
%right C220_KEY_POST_ID
%left  '[' ']'

%%

program:	/* empty */
              | program one_decl
	      | program main {ece220_main = $2;}
	      ;

declarations:	/* empty */
	      | declarations one_decl
	      ;

one_decl:	type var_list ';' ;

type:		C220_KEY_INT_TYPE ;

var_list:	variable
	      | var_list ',' variable
	      ;

variable:	ident array_sizes { var_create ($1, $2); $$ = NULL;} ;

ident:		C220_KEY_IDENT {$$ = new_ast_str (ece220token.name);} ;

array_sizes:	/* empty */ {$$ = new_ast_num (0);}
	      | '[' number ']' {$$ = $2;}
	      ;

number:		C220_KEY_INT { $$ = new_ast_num (ece220token.ival); } ;

main:		main_open '{' declarations statements '}' {
		    reading_globals = 0;
		    $$ = $4;
		}
	      ;

main_open:	type C220_KEY_MAIN '(' ')' {start_main ();} ;

statements:	/* empty */ {$$ = NULL;}
	      | statements stmt {
		    /* slow for long code, but simple... */
		    if (NULL == $1) {
		        $$ = $2;
		    } else {
			ast220_t* find;

			for (find = $1; NULL != find->next; find = find->next);
			find->next = $2;
			$$ = $1;
		    }
		}
	      ;

stmt:		simple_stmt {$$ = $1;}
	      | compound_stmt {$$ = $1;}
	      ;

simple_stmt:	maybe_expr ';' {$$ = $1;}
	      | C220_KEY_IF '(' expr ')' compound_stmt maybe_else {
	            $$ = new_ast_node ();
		    $$->type = AST220_IF_STMT;
		    $$->test = $3;
		    $$->left = $5;
		    $$->right = $6;
		}
	      | C220_KEY_FOR '(' maybe_expr ';' expr ';' maybe_expr ')'
					    compound_stmt { 
	            $$ = new_ast_node ();
		    $$->type = AST220_FOR_STMT;
		    $$->test = $5;
		    $$->left = $3;
		    $$->middle = $9;
		    $$->right = $7;
		}
	      | C220_KEY_RETURN expr ';' {
	            $$ = new_ast_node ();
		    $$->type = AST220_RETURN_STMT;
		    $$->left = $2;
	        }
	      | C220_KEY_DEBUG_MARKER '(' number ')' {
	            $$ = $3;
		    $$->type = AST220_DEBUG_MARKER;
	        }
	      ;

maybe_else:	/* empty */ {$$ = NULL;}
	      | C220_KEY_ELSE compound_stmt {$$ = $2;}
	      ;

compound_stmt:	'{' statements '}' {$$ = $2;} ;

maybe_expr:	/* empty */ {$$ = NULL;}
	      | expr {
	      	    $$ = new_ast_node (); 
		    $$->type = AST220_POP_STACK; 
		    $$->left = $1;
		}
	      ;

expr:		lval '=' expr {
	            $$ = new_ast_node ();
		    $$->type = AST220_OP_ASSIGN;
		    $$->left = $1;
		    $$->right = $3;
		}
	      | lval C220_KEY_INCR %prec C220_KEY_POST_ID {
	            $$ = new_ast_op (AST220_OP_POST_INCR, $1, NULL);
	        }
	      | lval C220_KEY_DECR %prec C220_KEY_POST_ID {
	            $$ = new_ast_op (AST220_OP_POST_DECR, $1, NULL);
	        }
	      | C220_KEY_INCR lval {
	            $$ = new_ast_op (AST220_OP_PRE_INCR, $2, NULL);
	        }
	      | C220_KEY_DECR lval {
	            $$ = new_ast_op (AST220_OP_PRE_DECR, $2, NULL);
	        }
	      | var_ref {$$ = $1;}
	      | '&' lval {
	            $$ = new_ast_node ();
		    $$->type = AST220_GET_ADDRESS;
		    $$->left = $2;
		}
	      | fn_call {$$ = $1;}
	      | '-' expr %prec C220_KEY_UNARY_NEG {
	            $$ = new_ast_op (AST220_OP_NEGATE, $2, NULL);
	        }
	      | '!' expr {
	            $$ = new_ast_op (AST220_OP_LOG_NOT, $2, NULL);
	        }
	      | expr '+' expr {
	            $$ = new_ast_op (AST220_OP_ADD, $1, $3);
	        }
	      | expr '-' expr {
	            $$ = new_ast_op (AST220_OP_SUB, $1, $3);
	        }
	      | expr '*' expr {
	            $$ = new_ast_op (AST220_OP_MULT, $1, $3);
	        }
	      | expr '/' expr {
	            $$ = new_ast_op (AST220_OP_DIV, $1, $3);
	        }
	      | expr '%' expr {
	            $$ = new_ast_op (AST220_OP_MOD, $1, $3);
	        }
	      | expr C220_KEY_NE expr {
	            $$ = new_ast_op (AST220_CMP_NE, $1, $3);
	        }
	      | expr '<' expr {
	            $$ = new_ast_op (AST220_CMP_LESS, $1, $3);
	        }
	      | expr C220_KEY_LE expr {
	            $$ = new_ast_op (AST220_CMP_LE, $1, $3);
	        }
	      | expr C220_KEY_EQ expr {
	            $$ = new_ast_op (AST220_CMP_EQ, $1, $3);
	        }
	      | expr C220_KEY_GE expr {
	            $$ = new_ast_op (AST220_CMP_GE, $1, $3);
	        }
	      | expr '>' expr {
	            $$ = new_ast_op (AST220_CMP_GREATER, $1, $3);
	        }
	      | expr C220_KEY_AND expr {
	            $$ = new_ast_op (AST220_OP_LOG_AND, $1, $3);
	        }
	      | expr C220_KEY_OR expr {
	            $$ = new_ast_op (AST220_OP_LOG_OR, $1, $3);
	        }
	      | number {$$ = $1; $$->type = AST220_PUSH_INT;}
	      | '(' expr ')' {$$ = $2;}
	      ;

var_ref:	ident array_index {
		    symtab_entry_t* entry = symtab_lookup ($1->name);

		    if (NULL == entry ||
			(0 == entry->array_len && NULL != $2) ||
			(0 != entry->array_len && NULL == $2)) {
			ece220errorf ("invalid variable access");
			/* should discard AST nodes here... */
			$$ = NULL;
		    } else {
			$$ = $1; 
			$$->type = AST220_VARIABLE;
			$$->left = $2;
		    }
		} 
	      ;

array_index:	/* empty */ {$$ = NULL;}
	      | '[' expr ']' {$$ = $2;}
	      ;

lval:		var_ref {$$ = $1;} ;

fn_call:	fn_name '(' maybe_args ')' {
		    $$ = $1; 
		    $$->type = AST220_FUNC_CALL;
		    $$->left = $3;
		} 
	      ;

fn_name:	C220_KEY_PRINTF {$$ = new_ast_func (AST220_PRINTF);}
	      | C220_KEY_RAND   {$$ = new_ast_func (AST220_RAND);}
	      | C220_KEY_SCANF  {$$ = new_ast_func (AST220_SCANF);}
	      | C220_KEY_SRAND  {$$ = new_ast_func (AST220_SRAND);}
	      ;

maybe_args:	/* empty */ {$$ = NULL;}
	      | arg_list {$$ = $1;}
	      ;

arg_list:	str_or_expr {$$ = $1;}
	      | arg_list ',' str_or_expr {
	            $3->next = $1;
		    $$ = $3;
		}
	      ;

str_or_expr:	expr {$$ = $1;}
	      | string {$$ = $1; $$->type = AST220_PUSH_STR;}
	      ;

string:		C220_KEY_STRING {$$ = new_ast_str (ece220token.name);} ;

%%

ast220_t*
ece220_parse_program ()
{
    ece220_reset_lexer ();
    ece220_main = NULL;
    reading_globals = 1;
    global_offset = 0;
    local_offset = 0;
    has_main = 0;
    ece220parse ();
    return ece220_main;
}

static void
var_create (ast220_t* name, ast220_t* a_len)
{
    int32_t var_size;
    symtab_entry_t* entry;

    entry = symtab_create (name->name);
    if (NULL == entry) {
	ece220errorf ("can't create variable: %s", name->name);
    } else {
	entry->array_len = a_len->value;
	entry->is_global = reading_globals;
	var_size = (a_len->value == 0 ? 1 : a_len->value);
	if (reading_globals) {
	    entry->offset = global_offset;
	    global_offset += var_size;
	} else {
	    entry->offset = local_offset - (var_size - 1);
	    local_offset -= var_size;
	}
    }
    free (name->name);
    free (name);
    free (a_len);
}

static void 
start_main ()
{
    reading_globals = 0;
    if (has_main) {
        ece220errorf ("multiple main functions");
    } else {
        has_main = 1;
    }
}

static ast220_t* 
new_ast_node ()
{
    ast220_t* node;

    /* not good practice to terminate in the middle of the program... */
    if (NULL == (node = malloc (sizeof (ast220_t)))) {
        perror ("panic");
	exit (3);
    }
    return node;
}

static ast220_t* 
new_ast_num (int32_t value)
{
    ast220_t* node;

    node = new_ast_node ();
    node->value = value;
    return node;
}

static ast220_t* 
new_ast_str (char* str)
{
    ast220_t* node;

    node = new_ast_node ();
    node->name = str;
    return node;
}

static ast220_t* 
new_ast_func (ast220_builtin_func_t id)
{
    ast220_t* node;

    node = new_ast_node ();
    node->fnum = id;
    return node;
}

static ast220_t* 
new_ast_op (ast220_type_t type, ast220_t* left, ast220_t* right)
{
    ast220_t* node;

    node = new_ast_node ();
    node->type = type;
    node->left = left;
    node->right = right;
    return node;
}

int32_t
ece220_local_var_space ()
{
    return -local_offset;
}

int32_t
ece220_global_var_space ()
{
    return global_offset;
}

