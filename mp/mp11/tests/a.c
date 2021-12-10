int array[8];

int 
main ()
{
    int i;

    for (i = 0; 8 > i; i++) {
        array[i] = i * i;
    }

    for (i = 0; 8 > i; i++) {
        printf ("array[%d] = %d\n", i, array[i]);
    }

    return 42;
}

