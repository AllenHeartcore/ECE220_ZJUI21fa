/*  ###### Copyright Information ######
    This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem IV. 
    Written and commented by Chen, Ziyuan on 17 October 2021. 
    Debugged and committed by Chen, Ziyuan on 18 October 2021. 
    Copyright © 2021 Chen, Ziyuan. All rights reserved. 

    ###### Functionality ######
    Compile by running:
        gcc -g -Wall main.c mp4.c -o stamps
    Execute by running:
        ./stamps <amount> <s1> <s2> <s3> <s4>
    The program outputs:
        <s1_best> <s2_best> <s3_best> <s4_best>
    which constitutes the best scheme of stamps. 
    The best scheme is determined as follows:
        (1) Total value of stamps in scheme is no less than <amount>; 
        (2) If ties occur in (1), select the scheme whose total value exceeds <amount> the least; 
        (3) If ties occur in (2), select the scheme with the least total number of stamps; 
        (4) If ties occur in (3), select the scheme with the largest <s1_best>; 
        (5) If ties occur in (4), select the scheme with the largest <s2_best>; 
        (6) If ties occur in (5), select the scheme with the largest <s3_best>. 
*/

#include <stdio.h>
#include <stdlib.h>
#include "mp4.h"

int32_t print_stamps (int32_t amount, int32_t s1, int32_t s2, int32_t s3, int32_t s4) {

    // Ex-loop initialization
    int32_t s1_best = 0, s2_best = 0, s3_best = 0, s4_best = 0;
    int32_t amount_diff_best = 2147483647, scount_best = 2147483647;

    for (uint8_t mask = 1; mask <= 15; mask++) {    // <mask> is in the format of 0b----

        // In-loop initialization
        int32_t s1_scheme = 0, s2_scheme = 0, s3_scheme = 0, s4_scheme = 0, smallest = 0;
        int32_t amount_trial = amount;
        uint8_t mask_trial = mask;

        // Generate scheme
        if (mask_trial % 2 == 1) {
            s1_scheme = amount_trial / s1;
            amount_trial %= s1;
            smallest = 1;
        }
        mask_trial >>= 1;
        if (mask_trial % 2 == 1) {
            s2_scheme = amount_trial / s2;
            amount_trial %= s2;
            smallest = 2;
        }
        mask_trial >>= 1;
        if (mask_trial % 2 == 1) {
            s3_scheme = amount_trial / s3;
            amount_trial %= s3;
            smallest = 3;
        }
        mask_trial >>= 1;
        if (mask_trial % 2 == 1) {
            s4_scheme = amount_trial / s4;
            amount_trial %= s4;
            smallest = 4;
        }

        // Handle inexact matchs
        if (amount_trial != 0) {
            if (smallest == 1) s1_scheme++;
            if (smallest == 2) s2_scheme++;
            if (smallest == 3) s3_scheme++;
            if (smallest == 4) s4_scheme++;
        }

        // Compare scheme
        int32_t value = s1 * s1_scheme + s2 * s2_scheme + s3 * s3_scheme + s4 * s4_scheme;
        int32_t scount = s1_scheme + s2_scheme + s3_scheme + s4_scheme;
        uint8_t update = 0;
        if (value - amount < amount_diff_best) update = 1;      // Solves ties in (1)
        else if (value - amount == amount_diff_best) {
            if (scount < scount_best) update = 1;               // Solves ties in (2)
            else if (scount == scount_best) {
                if (s1_scheme > s1_best) update = 1;            // Solves ties in (3)
                else if (s1_scheme == s1_best) {
                    if (s2_scheme > s2_best) update = 1;        // Solves ties in (4)
                    else if (s2_scheme == s2_best) {
                        if (s3_scheme > s3_best) update = 1;    // Solves ties in (5)
                    }
                }
            }
        }

        // Update the best scheme
        if (update == 1) {
            s1_best = s1_scheme;
            s2_best = s2_scheme;
            s3_best = s3_scheme;
            s4_best = s4_scheme;
            amount_diff_best = value - amount;
            scount_best = scount;
        }

    }

    // Output best scheme
    printf("%d %d %d %d -> %d\n", s1_best, s2_best, s3_best, s4_best, amount + amount_diff_best);
    return !amount_diff_best;

}