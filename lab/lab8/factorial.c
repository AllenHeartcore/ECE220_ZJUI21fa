#include <stdio.h>

long 
factorial (int n)
{
    int result = 1;

    while (n--) {
        result *= n;
    }
    return result;
}

int 
main ()
{
    int n;

    printf ("Enter a number\n");
    scanf ("%d", &n);

    long val = factorial (n);

    printf ("%ld\n", val);

    return 0;
}
