#include <ncurses.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "lab6.h"


#define NUM_WALLS  ((WORLD_SIZE) / 4)
#define NUM_GOLD   10
#define NUM_SNAKES 10


// the world
static int32_t* world      = NULL;
static int32_t* world_seen = NULL;

// check contents at a location in the world
int32_t
world_has (int32_t xpos, int32_t ypos)
{
    if (0 > xpos || WORLD_WIDTH <= xpos || 0 > ypos || WORLD_HEIGHT <= ypos) {
        return -1;
    }
    return world[ypos * WORLD_WIDTH + xpos];
}


// change the world; item_type must be one of the (non-negative) values 
// returned by world_has; returns 0 on failure, or 1 on success
int32_t 
world_set (int32_t xpos, int32_t ypos, int32_t item_type)
{
    if (0 > xpos || WORLD_WIDTH <= xpos || 0 >= ypos || WORLD_HEIGHT <= ypos ||
    	0 > item_type || NUM_WORLD_TYPES <= item_type) {
        return 0;
    }
    world[ypos * WORLD_WIDTH + xpos] = item_type;
    return 1;
}


static void
mark_seen (int32_t xpos, int32_t ypos)
{
    int32_t dx;
    int32_t dy;
    int32_t bound;

    for (dy = -2; 2 >= dy; dy++) {
        bound = 2 - (0 > dy ? -dy : dy);
        for (dx = -bound; bound >= dx; dx++) {
	    if (-1 <= (ypos + dy) && WORLD_HEIGHT >= (ypos + dy) &&
		-1 <= (xpos + dx) && WORLD_WIDTH >= (xpos + dx)) {
	        world_seen[(ypos + dy + 1) * (WORLD_WIDTH + 2) + 
			   xpos + dx + 1] = 1;
	    }
	}
    }
}


// place a fixed number of a particular item type
static void
place_world_item (int32_t* world, int32_t item_type, int32_t count)
{
    int32_t loc;

    while (0 < count--) {
        do {
	    loc = (rand () % WORLD_SIZE);
	} while (WORLD_EMPTY != world[loc]);
	world[loc] = item_type;
    }
}


// returns new world on success, NULL on failure
static int32_t*
make_world (void)
{
    int32_t* world;

    // I'm going to be lazy about world generation.  We'll just plop
    // down some walls, some gold, and some snakes.  Some worlds may
    // be impossible to win.  We could check that and regenerate, but
    // we won't.  One can't always win! :)  --SSL

    // Validate world size--could be done statically, too--then allocate.
    if (WORLD_SIZE < NUM_WALLS + NUM_GOLD + NUM_SNAKES ||
	NULL == (world = (int32_t*)calloc (WORLD_SIZE, sizeof (*world))) ||
	NULL == (world_seen = (int32_t*)calloc 
		 ((WORLD_HEIGHT + 2) * (WORLD_SIZE + 2), 
		 sizeof (*world_seen)))) {
    	// too small, or allocations failed
	if (NULL != world) {
	    free (world);
	}
	return 0;
    }

    // add some walls ... 25% of area (round down)
    place_world_item (world, WORLD_WALL, NUM_WALLS);

    // place some gold -- be nice and make 10 instead of 5
    place_world_item (world, WORLD_GOLD, NUM_GOLD);

    // place some snakes
    place_world_item (world, WORLD_SNAKE, NUM_SNAKES);

    // start player at (0,0)
    mark_seen (get_player_x (), get_player_y ());

    // no snakes at starting point
    world[0] = WORLD_EMPTY;
    world[1] = WORLD_EMPTY;
    world[WORLD_WIDTH] = WORLD_EMPTY;

    return world;
}


static void
print_world (void)
{
    int32_t px = get_player_x ();
    int32_t py = get_player_y ();
    int32_t i;
    int32_t j;

    clear ();
    for (j = -1; WORLD_HEIGHT >= j; j++) {
	for (i = -1; WORLD_WIDTH >= i; i++) {
	    if (!world_seen[(j + 1) * (WORLD_WIDTH + 2) + i + 1]) {
	        printw ("#");
	    } else if (0 > j || WORLD_HEIGHT <= j || 
	               0 > i || WORLD_WIDTH <= i) {
	        printw ("%%");
	    } else if (j == py && i == px) {
	        printw ("!");
	    } else switch (world[j * WORLD_WIDTH + i]) {
		case WORLD_EMPTY:
		    printw (" ");
		    break;
	        case WORLD_WALL:
		    printw ("%%");
		    break;
	        case WORLD_GOLD:
		    printw ("*");
		    break;
	        default:
		    printw ("S");
		    break;
	    }
	}
	printw ("\n");
    }
    printw ("\n--- Use AWSD to move.  You are !  Collect gold *.  Avoid snakes S.\n");
    printw ("    Current Gold: %d    Press Q to quit.\n", player_has_gold ());
    refresh ();
}


static void
play_game (void)
{
    int32_t rval;

    while (1) {
	print_world ();
	do {
	    switch (getch ()) {
		case 'a':
		    rval = player_move_left ();
		    break;
		case 'w':
		    rval = player_move_up ();
		    break;
		case 's':
		    rval = player_move_down ();
		    break;
		case 'd':
		    rval = player_move_right ();
		    break;
		case 'q':
		    printw ("\n\nQuitter!\n");
		    return;
		default:
		    rval = 0;
		    break;
	    }
	} while (0 == rval);
	mark_seen (get_player_x (), get_player_y ());
	if (2 == rval) {
	    print_world ();
	    printw ("\n\nCongratulations!  You have won!\n");
	    return;
	}
	if (3 == rval) {
	    print_world ();
	    printw ("\n\nOuch!  A snake bit you!\n");
	    return;
	}
    }	
}


int
main ()
{
    // randomize
    srand (time (NULL));

    if (NULL == (world = make_world ())) {
        fprintf (stderr, "Could not make world!\n");
	return 3;
    }

    initscr ();
    noecho ();
    raw ();
    refresh ();

    play_game ();

    printw ("\n\nPress any key to quit.\n");
    refresh ();
    halfdelay (1);
    sleep (1);                  // wait a second
    while (ERR != getch ()) { } // flush buffer
    while (ERR == getch ()) { } // wait for keystroke
    endwin ();

    return 0;
}

