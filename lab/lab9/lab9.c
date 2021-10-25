#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "mp5.h"


static void
build_tree (int32_t level, int32_t x, int32_t y, double angle, double width)
{
    // you need to write this code
    // don't forget to set the color before drawing anything!
}

int
main ()
{
    // this random seed produces the tree in the lab specification
    srand48 (0x5BDE78CB);

    // for random trees, uncomment the line below
    // srand48 (time (NULL));

    printf ("#include <stdint.h>\n\n#include \"mp5.h\"\n\nvoid\n");
    printf ("draw_tree (void) {\n");
    build_tree (0, WIDTH / 2, HEIGHT - 1, M_PI_2, 20);
    printf ("}\n");
    
    return 0;
}

