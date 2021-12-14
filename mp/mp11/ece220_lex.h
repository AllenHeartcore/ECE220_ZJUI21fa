/*									tab:8
 *
 * ece220_lex.h - lexer header file for an ECE190 compiler MP (F08 MP5)
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
 * Creation Date:   14 October 2008
 * Filename:	    ece220_lex.f
 * History:
 *	SL	2	Tue Apr 03 23:15:59 2018
 *		Changed 190 to 220 for ECE220.
 *	SSL	1	14 October 2008
 *		Basic token and error functionality.
 */

#if !defined(ECE220_LEX_H)
#define ECE220_LEX_H

#include <stdint.h>

typedef struct ece220_token_t ece220_token_t;
struct ece220_token_t {
    int16_t ival; /* for integer constants and variables */
    char* start;
    char* name;
};

/* variable appears in ece220_lex.y */
extern ece220_token_t ece220token;

extern void ece220_reset_lexer ();
extern void ece220error (const char* msg);
extern void ece220errorf (const char* fmt, ...);

#endif /* ECE220_LEX_H */

