/*  ###### Copyright Information ######
    This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem IV. 
    Written and commented by Chen, Ziyuan on 17 October 2021. 
    Debugged and committed by Chen, Ziyuan on 18 October 2021. 
    Refined and re-committed by Chen, Ziyuan on 19 October 2021. 
    Copyright © 2021 Chen, Ziyuan. All rights reserved. 

    ###### Functionality ######
    Compile by running:
        gcc -g -Wall -O3 main.c mp4.c -o stamps
    Execute by running:
        ./stamps <amount> <s1> <s2> <s3> <s4>
    The program outputs:
        <s1_best> <s2_best> <s3_best> <s4_best> -> <amount + amount_diff_best> (aka. <amount_best_match>)
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
#define LARGEST_INT32 0b01111111111111111111111111111111

int32_t print_stamps (int32_t amount, int32_t s1, int32_t s2, int32_t s3, int32_t s4) {
    // Ex-loop initialization (best records)
    int32_t s1_best = 0, s2_best = 0, s3_best = 0, s4_best = 0;
    int32_t amount_diff_best = LARGEST_INT32, scount_best = LARGEST_INT32;
    // Generate schemes
    for (int32_t s1_scheme = 0; s1_scheme <= amount / s1; s1_scheme++) {
        for (int32_t s2_scheme = 0; s2_scheme <= ((amount - s1 * s1_scheme) / s2); s2_scheme++) {
            for (int32_t s3_scheme = 0; s3_scheme <= ((amount - s1 * s1_scheme - s2 * s2_scheme) / s3); s3_scheme++) {
                int32_t s4_scheme = (amount - s1 * s1_scheme - s2 * s2_scheme - s3 * s3_scheme) / s4;
                // Generate criteria
                int32_t value = s1 * s1_scheme + s2 * s2_scheme + s3 * s3_scheme + s4 * s4_scheme;
                int32_t scount = s1_scheme + s2_scheme + s3_scheme + s4_scheme;
                int32_t amount_diff = value - amount;
                // Inexact match handling
                if (amount_diff < 0) {
                    s4_scheme++;
                    amount_diff += s4;
                    scount++;
                }
                // Compare schemes
                uint8_t update = 0;
                if (amount_diff < amount_diff_best) update = 1;
                else if (amount_diff == amount_diff_best) {
                    if (scount <= scount_best) update = 1;
                }
                // Update the best scheme
                if (update == 1) {
                    s1_best = s1_scheme;
                    s2_best = s2_scheme;
                    s3_best = s3_scheme;
                    s4_best = s4_scheme;
                    amount_diff_best = amount_diff;
                    scount_best = scount;
                }
            }
        }
    }
    // Output the best scheme
    printf("%d %d %d %d -> %d\n", s1_best, s2_best, s3_best, s4_best, amount + amount_diff_best);
    return !amount_diff_best;
}