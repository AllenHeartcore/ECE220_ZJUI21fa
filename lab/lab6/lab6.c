#include <stdint.h>

#include "lab6.h"


// file-scope variables to hold the player's data

static int32_t px = 0;    // player's X position (from 0 to WORLD_WIDTH - 1)
static int32_t py = 0;    // player's Y position (from 0 to WORLD_HEIGHT - 1)
static int32_t pgold = 0; // player's gold       (collect 5 to win the game)


// simple access functions written for you; these functions allow 
// code in main.c to obtain the values of the file-scope variables

int32_t get_player_x (void) { return px; }
int32_t get_player_y (void) { return py; }
int32_t player_has_gold (void) { return pgold; }


//
// YOU MUST WRITE THE FIVE FUNCTIONS BELOW
//
// The specification explains what each should do.
//
// Return values have the following meanings for all five functions...
//    0 -- move failed (move outside world, move into wall)
//    1 -- move successful
//    2 -- player won the game by collecting 5 gold
//    3 -- player died by moving next to a snake
//

int32_t
player_move_left (void)
{
    return 0;
}

int32_t
player_move_right (void)
{
    return 0;
}

int32_t
player_move_up (void)
{
    return 0;
}

int32_t
player_move_down (void)
{
    return 0;
}

int32_t 
check_new_move (void)
{
    return 1;
}

