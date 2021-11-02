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
    
    int32_t* array_ptr = &array;
    for (int init = 0; init < 105; init++) *(array_ptr + init) = init;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 7; k++) {
                printf("%3d ", array[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}


