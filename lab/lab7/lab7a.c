#include <stdint.h>
#include <stdio.h>


static void
show_mapping (int32_t A[3][5][7], int32_t* B)
{
    printf ("A = %p, B = %p\n\n", A, B);
}


int
main ()
{
    int32_t array[3][5][7];

    show_mapping (array, (int32_t*)array);
    
    return 0;
}


