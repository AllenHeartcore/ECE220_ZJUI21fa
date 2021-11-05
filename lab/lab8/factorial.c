#include <stdio.h>

long 
factorial (int n)
{
    int result = 1;

    n += 1;
    while (--n) {
        result *= n;
    }
    return result;
}

int 
main ()
{
    int m;

    printf ("Enter a number\n");
    scanf ("%d", &m);

    long val = factorial (m);

    printf ("%ld\n", val);

    return 0;
}
