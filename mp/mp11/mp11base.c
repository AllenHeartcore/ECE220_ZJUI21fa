/*									tab:8
 *
 * mp11base.c - wrapper code and library insertion for ECE220 compiler (F08 MP5)
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
 * Creation Date:   15 October 1008
 * Filename:	    mp11base.c
 * History:
 *	SL	2	Tue Apr 03 23:24:13 2018
 *		Changed MP5 to MP11 and 190 to 220 for ECE220.
 *	SL	1	15 October 1008
 *		First written.
 */


#include <stdio.h>

#include "ece220_parse.h"
#include "ece220_symtab.h"


static int32_t dump_prefix ();
static int32_t dump_suffix ();
static int32_t dump_file (const char* fname);

extern void MP11_generate_code (ast220_t* prog);


/*
 * write the whole assembly file out; call to generate main's instructions 
 * in the middle
 */
int
main ()
{
    ast220_t* program;

    if (NULL == (program = ece220_parse_program ())) {
        /* parse failed; stop now */
	return 3;
    }

    if (-1 == dump_prefix ()) {
        return 3;
    }
    symtab_dump ();
    MP11_generate_code (program);
    if (-1 == dump_suffix ()) {
        return 3;
    }
    return 0;
}


/* copy the student code prefix to stdout */

static int32_t
dump_prefix ()
{
    int32_t amt;

    if (-1 == dump_file ("skeleton/part1.asm")) {
        return -1;
    }
    for (amt = ece220_local_var_space (); 16 <= amt; amt -= 16) {
	printf ("\tADD R6,R6,#-16\n");
    }
    if (0 < amt) {
	printf ("\tADD R6,R6,#-%d\n", amt);
    }
    return dump_file ("skeleton/part2.asm");
}


/* copy the student code suffix (library, etc.) to stdout */

static int32_t
dump_suffix ()
{
    if (-1 == dump_file ("skeleton/part3.asm")) {
        return -1;
    }
    printf ("\t.BLKW #%d\n", ece220_global_var_space ());
    dump_file ("skeleton/part4.asm");
    return 0;
}


/* copy a file to stdout */

static int32_t
dump_file (const char* fname)
{
    FILE* f;
    char buf[65536];
    int32_t amt;

    if (NULL == (f = fopen (fname, "r"))) {
	perror ("open skeleton assembly file");
	fprintf (stderr, "filename is %s\n", fname);
        return -1;
    }
    while (1) {
	amt = fread (buf, sizeof (*buf), sizeof (buf), f);
	if (0 >= amt) {
	    fclose (f);
	    return 0;
	}
	if (amt != fwrite (buf, sizeof (*buf), amt, stdout)) {
	    fclose (f);
	    return -1;
	}
    }
}


