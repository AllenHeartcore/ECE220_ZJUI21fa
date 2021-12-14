/* 
 * this is a test program for the ECE190 F08 MP5 compiler
 */

int piles[3];

int
main ()
{
    int seed;
    int i;
    int j;
    int done;
    int choice_ok;
    int pnum;
    int amt;

    printf ("Please type a number: ");
    if (1 != scanf ("%d", &seed)) {
        printf ("That's not a number!\n");
	return 3;
    }
    srand (seed);

    for (i = 0; 3 > i; i++) {
	piles[i] = (rand () % 10) + 5;
    }

    for (done = 0; 0 == done;) {
	for (i = 0; 3 > i; i++) {
	    printf ("Pile %d: ", i + 1);
	    for (j = 0; piles[i] > j; j++) {
	        printf ("*");
	    }
	    printf ("\n");
	}
	for (choice_ok = 0; 0 == choice_ok; ) {
	    printf ("From which pile will you take sticks? ");
	    if (1 != scanf ("%d", &pnum) || 1 > pnum || 3 < pnum ||
	        0 == piles[pnum - 1]) {
	        printf ("That's not a good choice.\n");
	    } else {
	        choice_ok = 1;
		pnum = pnum - 1;
	    }
	}
	for (choice_ok = 0; 0 == choice_ok; ) {
	    printf ("How many sticks will you take from pile %d? ", pnum + 1);
	    if (1 != scanf ("%d", &amt) || 0 >= amt || piles[pnum] < amt) {
	        printf ("That's not a good choice.\n");
	    } else {
	        choice_ok = 1;
	    }
	}
	piles[pnum] = piles[pnum] - amt;
        if (0 == piles[0] && 0 == piles[1] && 0 == piles[2]) {
	    printf ("You win!\n");
	    done = 1;
	} else {
	    for (pnum = (rand () % 3); 0 == piles[pnum]; pnum = (rand () % 3)) {
	    }
	    amt = (rand () % piles[pnum]) + 1;
	    printf ("I take %d from pile %d\n", amt, pnum + 1);
	    piles[pnum] = piles[pnum] - amt;
	    if (0 == piles[0] && 0 == piles[1] && 0 == piles[2]) {
		printf ("I win!\n");
		done = 1;
	    }
	}
    }

    return 0;
}

