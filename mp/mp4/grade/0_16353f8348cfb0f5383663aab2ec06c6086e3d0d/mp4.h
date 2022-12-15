#if !defined(MP4_H)
#define MP4_H

#include <stdint.h>

/*
 * The print_stamps function prints the combination of stamps with minimum
 * value but at least 'amount'.  If multiple combinations are possible,
 * print_stamps prints the combination with the fewest stamps.  If ties
 * are still possible, print_stamps maximizes the number of the largest
 * stamp (s1), then the next largest, and so forth.
 *
 * - amount must be positive, as must all stamp values
 * - also, stamp values s1 through s4 must be in strictly monotonically 
 *   decreasing order (no ties)
 *
 * Returns 1 if the printed combination matches the 'amount', or 0 if the
 * printed combination is more than the amount.
 */
extern int32_t
print_stamps (int32_t amount, int32_t s1, int32_t s2, int32_t s3, int32_t s4);

#endif /* MP4_H */
