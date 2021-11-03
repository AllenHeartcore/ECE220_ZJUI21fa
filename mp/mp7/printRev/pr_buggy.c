/*
 * ECE220 Spring 2018
 *
 * Program name: pr_buggy.c, a recursive backwards string printer
 *
 * Description: This program requests a string, then prints it in 
 *              reverse along with its length.  The main work is performed
 *              by the print_reverse subroutine.
 *
 * History: adapted from ECE190 notes ca. 2005
 *          added bug for MP7
 */


/* 
 * DISCLAIMER: In this file, we use 'int32_t' for clarity on integer
 * sizes in C code.  However, when we think about the LC-3 stack frames 
 * corresponding to these functions, we will instead treat all integers
 * as 16-bit for simplicity.
 */

#include <stdint.h>       /* Include C's standard integer header file.      */
#include <stdio.h>        /* Include C's standard I/O header file.          */


/*
 * print_reverse -- print a string in reverse and return its length
 * INPUT -- s, the string to be printed in reverse
 * OUTPUT -- none
 * RETURN VALUE -- length of original string
 * SIDE EFFECTS -- prints the string s in reverse
 */
int32_t
print_reverse (const char* s)
{
    int32_t rest;

    // For an empty string, simply return 0.  This test is the stopping
    // condition for the recursion.
    if ('\0' == *s) {
        return rest;
    }

    // Recurse on the rest of the string, printing it and finding its length.
    rest = print_reverse (s + 1);

    // Print this call's character (all later characters already printed).
    printf ("%c", *s);

    // Length of this string is the length of the rest of the string
    // (from the recursive call) plus one.
    return (rest + 1);
}


