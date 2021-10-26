#if !defined(LAB6_H)
#define LAB6_H

#include <stdint.h>

#define WORLD_WIDTH  20		// width of world
#define WORLD_HEIGHT 12		// height of world
#define WORLD_SIZE   ((WORLD_WIDTH) * (WORLD_HEIGHT))

enum {
    WORLD_EMPTY,
    WORLD_WALL,
    WORLD_GOLD,
    WORLD_SNAKE,
    NUM_WORLD_TYPES
};


///////////////////////////////////////////////////////////////////////////
//
// These are the functions given to you
//

// what's at the world at position (xpos, ypos)?
// return values include ...
//   -1 -- illegal position
//    0 -- empty   (you can use constant WORLD_EMPTY if desired)
//    1 -- a wall  (you can use constant WORLD_WALL if desired)
//    2 -- gold    (you can use constant WORLD_GOLD if desired)
//    3 -- a snake (you can use constant WORLD_SNAKE if desired)
extern int32_t world_has (int32_t xpos, int32_t ypos);

// change the world; item_type must be one of the (non-negative) values 
// returned by world_has; returns 0 on failure, or 1 on success
extern int32_t world_set (int32_t xpos, int32_t ypos, int32_t item_type);

///////////////////////////////////////////////////////////////////////////
//
// These are the functions that you must write.
//
// return value meanings are as follows:
//    0 -- move failed
//    1 -- move successful
//    2 -- player won the game by collecting 5 gold
//    3 -- player died by moving next to a snake
//
// See the Lab 6 specification for instructions on what each function does.

extern int32_t player_move_left (void);
extern int32_t player_move_right (void);
extern int32_t player_move_up (void);
extern int32_t player_move_down (void);
extern int32_t check_new_move (void);


///////////////////////////////////////////////////////////////////////////
//
// These functions are also in lab6.c, but they have been written for you.
//
extern int32_t get_player_x (void);
extern int32_t get_player_y (void);
extern int32_t player_has_gold (void);

#endif // LAB6_H
