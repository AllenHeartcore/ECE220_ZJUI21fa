#include <stdint.h>
#include <stdio.h>
#include "lab5.h"
#define MASK  0b10000000000000000000000000000000
#define MATCH 0b11111111111111111111111111111111
#define MUTEX 0b00000000000000000000000000000000

int32_t fit_pieces (uint32_t p1, uint32_t p2, uint32_t p3) {
    uint32_t p2_store = p2, p3_store = p3;
    uint8_t p1_overflow = 0;
    for (uint8_t p1_shift = 0; p1_shift <= 31; p1_shift++) {
        if (p1_overflow == 1) break;
        p2 = p2_store;
        uint8_t p2_overflow = 0;
        for (uint8_t p2_shift = 0; p2_shift <= 31; p2_shift++) {
            if (p2_overflow == 1) break;
            p3 = p3_store;
            uint8_t p3_overflow = 0;
            for (uint8_t p3_shift = 0; p3_shift <= 31; p3_shift++) {
                if (p3_overflow == 1) break;
                if (((p1 | p2 | p3) == MATCH) && ((p1 & p2 & p3) == MUTEX)) {
                    printf("Shift values are %2d, %2d and %2d.\n", p1_shift, p2_shift, p3_shift);
                    return 1;
                }
                if ((p3 & MASK) == MASK) p3_overflow = 1;
                p3 <<= 1;
            }
            if ((p2 & MASK) == MASK) p2_overflow = 1;
            p2 <<= 1;
        }
        if ((p1 & MASK) == MASK) p1_overflow = 1;
        p1 <<= 1;
    }
    return 0;
}