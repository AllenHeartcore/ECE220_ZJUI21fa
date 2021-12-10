/*									tab:8
 *
 * ece220_lex.f - lexer for an ECE190 compiler MP (F08 MP5)
 *
 * "Copyright (c) 2006-2018 by Steven S. Lumetta."
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
 * Creation Date:   29 September 2008
 * Filename:	    ece220_lex.f
 * History:
 *	SL	2	Tue Apr 03 23:15:33 2018
 *		Changed 190 to 220 for ECE220.
 *	SSL	1	29 September 2008
 *		Based on lexer for toy architecture.
 */

%option noyywrap nounput

%{

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ece220_parse.tab.h"
#include "ece220_lex.h"

/* not part of ANSI, but in gcclib */
extern char* strdup (const char* s);
extern int fileno (FILE* stream);
extern int vsnprintf (char* s, size_t len, const char* fmt, ...);


ece220_token_t ece220token;

static int16_t parse_int (const char* txt);

static int32_t line_num;

#define YY_NO_INPUT 1

%}


/* operand types */
DECIMAL  [0-9]+
SDECIMAL [-]?{DECIMAL}

IDENT	 [A-Za-z_][A-Za-z_0-9]*

/* operand and white space specification */
SPACE     [ \t]
ENDLINE   \r?\n\r?

/* 
 * exclusive lexing states to read comments and to skip input after error
 */
%x ls_comment ls_error

%%
    /* rules for numeric constants */
{SDECIMAL} {ece220token.ival = parse_int (yytext); return C220_KEY_INT;}

    /* rules for keywords */
else	{return C220_KEY_ELSE;}
for	{return C220_KEY_FOR;}
if	{return C220_KEY_IF;}
int	{return C220_KEY_INT_TYPE;}
return	{return C220_KEY_RETURN;}
DEBUG   {return C220_KEY_DEBUG_MARKER;}

    /* multi-character operators */
"&&"	{return C220_KEY_AND;}
"||"	{return C220_KEY_OR;}
"=="	{return C220_KEY_EQ;}
">="	{return C220_KEY_GE;}
"<="	{return C220_KEY_LE;}
"!="	{return C220_KEY_NE;}
"++"    {return C220_KEY_INCR;}
"--"    {return C220_KEY_DECR;}
    
    /* built-in "library" functions (simpler for students? or for parser?) */
main	{return C220_KEY_MAIN;}
printf	{return C220_KEY_PRINTF;}
rand	{return C220_KEY_RAND;}
scanf	{return C220_KEY_SCANF;}
srand	{return C220_KEY_SRAND;}

    /* 
     * rules for variables (must come after 
     * keywords to allow 2-character keywords)
     */
{IDENT} {ece220token.name = strdup (yytext); return C220_KEY_IDENT;}

"/*"           {
                   BEGIN (ls_comment);
	       }
<ls_comment>{ENDLINE} {line_num++;}
<ls_comment>"*/"      {BEGIN (0);}
<ls_comment>.         {}

\"[^\"\n\r]*\"	{ece220token.name = strdup (yytext); return C220_KEY_STRING;}
\"[^\"]		{ece220error ("unterminated string");}

    /* eat white space */
{SPACE}+ {}
{ENDLINE} {line_num++;}
. {return *yytext;}

    /* ignore any text after an error */
<ls_error>.         {}
<ls_error>{ENDLINE} {}
<ls_error><<EOF>>   {return -1;}

%%

static int16_t
parse_int (const char* txt)
{
    int32_t num;

    /* we allow only 16-bit values */
    if (1 != sscanf (txt, "%d", &num)) {
        ece220error ("bad numeric constant");
	return 0;
    }
    if (-32768 > num || 32767 < num) {
        ece220error ("numeric constant too large for 16 bits");
	return 0;
    }
    return num;
}

void 
ece220error (const char* msg)
{
    fprintf (stderr, "line %3d: %s\n", line_num, msg);
    BEGIN (ls_error);
}

void
ece220errorf (const char *fmt, ...) 
{
    char buf[200];
    va_list args;

    /* Check format parameter. */
    if (fmt == NULL)
        return;

    /* Append formatted string. */
    va_start (args, fmt);
    vsnprintf (buf, sizeof (buf), fmt, args);
    va_end (args);

    /* Send it to stderr. */
    fprintf (stderr, "line %3d: %s\n", line_num, buf);

    BEGIN (ls_error);
}

void
ece220_reset_lexer ()
{
    line_num = 1;
}

