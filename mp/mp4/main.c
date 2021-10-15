#include <stdio.h>
#include <stdlib.h>

#include "mp4.h"

int
main (int argc, const char* const argv[])
{
    int32_t amount; /* value needed */
    int32_t s1;    /* stamp type 1 value */
    int32_t s2;    /* stamp type 2 value */
    int32_t s3;    /* stamp type 3 value */
    int32_t s4;    /* stamp type 4 value */

    if (6 != argc) {
        fprintf (stderr, "syntax: %s <amount> <s1> <s2> <s3> <s4>\n", 
		 argv[0]);
	return 2;
    }

    amount = atoi (argv[1]);
    s1 = atoi (argv[2]);
    s2 = atoi (argv[3]);
    s3 = atoi (argv[4]);
    s4 = atoi (argv[5]);

    if (1 > amount || s1 <= s2 || s2 <= s3 || s3 <= s4 || 1 > s4) {
        fputs ("Amount must be positive.\n", stderr);
        fputs ("Stamp values must be positive and strictly decreasing.\n", 
	       stderr);
	return 2;
    }

    if (!print_stamps (amount, s1, s2, s3, s4)) {
        printf ("(exact match not possible)\n");
    }

    return 0;
}

