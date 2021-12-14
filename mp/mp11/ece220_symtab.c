/*									tab:8
 *
 * ece220_symtab.c - symbol table source file for ECE190 compiler (F08 MP5)
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
 * Creation Date:   Thu Oct 17 19:34:07 2008
 * Filename:	    ece220_symtab.c
 * History:
 *	SL	2	Tue Apr 03 23:21:55 2018
 *		Changed 190 to 220 for ECE220.
 *	SL	1	Thu Oct 17 19:34:07 2008
 *		First written.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ece220_symtab.h"

/* not part of ANSI, but in gcclib */
extern char* strdup (const char* s);

static symtab_entry_t* symtab = NULL;
static int32_t max_entries    = 10;
static int32_t num_entries    = 0;

symtab_entry_t* 
symtab_create (const char* vname)
{
    symtab_entry_t* new_entry;
    symtab_entry_t* new_copy;

    if (NULL != symtab_lookup (vname)) {
        return NULL;
    }
    if (NULL == symtab) {
	symtab = malloc (max_entries * sizeof (*symtab));
	if (NULL == symtab) {
	    return NULL;
	}
    } else if (max_entries == num_entries) {
        new_copy = realloc (symtab, 2 * max_entries * sizeof (*symtab));
	if (NULL == new_copy) {
	    return NULL;
	}
	symtab = new_copy;
	max_entries *= 2;
    }
    new_entry = &symtab[num_entries++];
    new_entry->name = strdup (vname);
    return new_entry;
}

symtab_entry_t* 
symtab_lookup (const char* vname)
{
    int32_t i;

    for (i = 0; num_entries > i; i++) {
        if (0 == strcmp (vname, symtab[i].name)) {
	    return &symtab[i];
	}
    }
    return NULL;
}

void
symtab_dump ()
{
    int32_t i;
    char vname[40];

    for (i = 0; num_entries > i; i++) {
	if (0 == symtab[i].array_len) {
	    sprintf (vname, "%20s", symtab[i].name);
	} else {
	    sprintf (vname, "%20s[%d]", symtab[i].name, symtab[i].array_len);
	}
        printf ("; %-30s %-15s offset=%+d\n", vname, 
		(symtab[i].is_global ? "global" : "local to main"), 
		symtab[i].offset);
    }
}

