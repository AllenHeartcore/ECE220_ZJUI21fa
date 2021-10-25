#if !defined(LAB5_H)
#define LAB5_H

#include <stdint.h>

/*
 * The fit_pieces function determines whether the "pieces" (bit patterns,
 * with some number of leading 0s) can "fit together" (line up with exactly
 * one 1 in any bit position, filling all 32 bits with 1s) when shifted
 * left by appropriate amounts (without overflow / loss of any 1 bits).
 *
 * If such shifts can be found, the function prints the left shift amounts
 * for each of the three pieces in order--p1, p2, and p3--then returns 1.
 * If no such shifts exist, the function returns 0.
 */
extern int32_t
fit_pieces (uint32_t p1, uint32_t p2, uint32_t p3);

#endif /* LAB5_H */
