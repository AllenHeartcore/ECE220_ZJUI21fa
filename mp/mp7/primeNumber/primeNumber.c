/*
 * ECE220 Spring 2018
 *
 * Program name: primes.c, a prime number printer
 *
 * Description: This program prints all primes under 1000.
 *
 * History: adapted from ECE190 notes ca. 2005
 */


/* 
 * DISCLAIMER: In this file, we use 'int32_t' for clarity on integer
 * sizes in C code.  However, when we think about the LC-3 stack frames 
 * corresponding to these functions, we will instead treat all integers
 * as 16-bit for simplicity.
 */

#include <stdio.h>        /* Include C's standard I/O header file.          */
#include <stdint.h>
#include "isPrime.h"

// divides_evenly returns true (non-zero) if divisor divides num evenly, 
// or false (0) otherwise
int32_t divides_evenly (int32_t divisor, int32_t value);



/* 
 * Function: main
 * Description: prints all primes under 1000
 * Parameters: none
 * Return Value: 0 on success
 */

int
main ()
{
    int32_t check;

    // 2 is the smallest prime number
    for (check = 2; 1000 > check; check++) {
        if (is_prime (check)) {
	    printf ("%d is prime.\n", check);
	}
    }

    // We're done.  Return success.
    return 0;
}

/* 
 * Function: divides_evenly
 * Description: checks whether one number divides another evenly or not
 * Parameters: divisor, the number used as a potential divisor
 *             value, the number being divided
 * Return Value: true (non-zero) if divisor divides value evenly, or 
 *               false (0) if it does not
 */

int32_t
divides_evenly (int32_t divisor, int32_t value)
{
    int32_t multiple;

    // Use integer arithmetic to check for remainder.
    multiple = (value / divisor) * divisor;

    return (multiple == value);
}

