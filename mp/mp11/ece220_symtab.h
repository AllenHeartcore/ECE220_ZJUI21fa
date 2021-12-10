/*									tab:8
 *
 * ece220_symtab.h - symbol table header file for ECE190 compiler (F08 MP5)
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
 * Version:	    2
 * Creation Date:   Thu Oct 17 19:30:29 2008
 * Filename:	    ece220_symbtab.c
 * History:
 *	SL	2	Tue Apr 03 23:19:22 2018
 *		Added 220 markings for ECE220.
 *	SL	1	Thu Oct 17 19:30:29 2008
 *		First written.
 */


#if !defined(ECE220_SYMTAB_H)
#define ECE220_SYMTAB_H


/* TYPE DEFINITION */

typedef struct symtab_entry_t symtab_entry_t;
struct symtab_entry_t {
    char* name;      /* variable name (case sensitive)              */
    int   array_len; /* size of array (0 = not an array)            */
    int   is_global; /* global variable or local to main            */
    int	  offset;    /* offset with respect to appropriate register */
};


/*
 * INTERFACE FUNCTIONS
 */

/* 
 * symtab_create -- create a new symbol table entry for a given variable
 * name; returns NULL on failure; used by the parser 
 */
extern symtab_entry_t* symtab_create (const char* vname);

/* 
 * symtab_lookup -- find the symbol table entry for a given variable
 * name; returns NULL on failure
 */
extern symtab_entry_t* symtab_lookup (const char* vname);

/*
 * symtab_dump -- print the symbol table to stdout
 */
extern void symtab_dump ();

#endif /* ECE220_SYMTAB_H */
