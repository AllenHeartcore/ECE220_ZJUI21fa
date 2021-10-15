#include <stdio.h>
#include <stdlib.h>

#include "lab5.h"

int
main (int argc, const char* const argv[])
{
    uint32_t p1;    /* puzzle piece 1 */
    uint32_t p2;    /* puzzle piece 2 */
    uint32_t p3;    /* puzzle piece 3 */

    if (4 != argc) {
        fprintf (stderr, "syntax: %s <p1> <p2> <p3>\n", argv[0]);
	return 2;
    }

    p1 = atoi (argv[1]);
    p2 = atoi (argv[2]);
    p3 = atoi (argv[3]);

    if (0 == p1 || 0 == p2 || 0 == p3) {
        fputs ("All pieces must be non-zero.\n", stderr);
	return 2;
    }

    if (1 != fit_pieces (p1, p2, p3)) {
        printf ("Impossible!\n");
	return 3;
    }

    return 0;
}

