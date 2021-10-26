#if !defined(LAB7_H)
#define LAB7_H

#include <stdint.h>

extern void edgeDetect 
    (int32_t width, int32_t height, 
     const int32_t* sobelX, const int32_t* sobelY,
     const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
     uint8_t* outMono, uint8_t* temp);

#endif // LAB7_H
