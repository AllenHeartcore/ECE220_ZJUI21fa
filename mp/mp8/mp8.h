#if !defined(MP8_H)
#define MP8_H

#include <stdint.h>

// Flood from (startX, startY) until meeting white (RGB=0xFFFFFF), filling
// with the specified RGB color.
extern void basicFlood 
    (int32_t width, int32_t height, const uint8_t* inRed, 
     const uint8_t* inGreen, const uint8_t* inBlue, int32_t startX, 
     int32_t startY, uint8_t floodR, uint8_t floodG, uint8_t floodB, 
     uint8_t* outRed, uint8_t* outGreen, uint8_t* outBlue);

// Returns 1 iff two colors are within Euclidean distance squared of 
// one another in RGB space.
extern int32_t colorsWithinDistSq 
    (uint8_t r1, uint8_t g1, uint8_t b1, 
     uint8_t r2, uint8_t g2, uint8_t b2, uint32_t distSq);

// Flood from (startX, startY) until meeting a color within distSq 
// (Euclidian distance squared in RGB space) of white (RGB=0xFFFFFF), filling
// with the specified RGB color.
extern void greyFlood 
    (int32_t width, int32_t height, const uint8_t* inRed, 
     const uint8_t* inGreen, const uint8_t* inBlue, int32_t startX, 
     int32_t startY, uint8_t floodR, uint8_t floodG, uint8_t floodB, 
     uint32_t distSq, uint8_t* outRed, uint8_t* outGreen, uint8_t* outBlue);

// Flood from (startX, startY) until meeting a color more than distSq 
// (Euclidian distance squared in RGB space) away from the color at
// the starting point, also stopping at 35 pixels from the starting point.
// Fill with the specified RGB color.
extern void limitedFlood 
    (int32_t width, int32_t height, const uint8_t* inRed,
     const uint8_t* inGreen, const uint8_t* inBlue, int32_t startX, 
     int32_t startY, uint8_t floodR, uint8_t floodG, uint8_t floodB, 
     uint32_t distSq, uint8_t* outRed, uint8_t* outGreen, uint8_t* outBlue);

// ------------------------------------------------------------------------
// The following routines are in mp8recurse.c, and are only called
// from the wrapper functions in mp8.c.

// called by basicFlood to perform marking of the flooded region
extern void basicRecurse
    (int32_t width, int32_t height, const uint8_t* inRed,
     const uint8_t* inGreen, const uint8_t* inBlue, int32_t x, int32_t y, 
     uint8_t* marking);
     
// called by greyFlood to perform marking of the flooded region
extern void greyRecurse 
    (int32_t width, int32_t height, const uint8_t* inRed, 
     const uint8_t* inGreen, const uint8_t* inBlue, int32_t x, int32_t y, 
     uint32_t distSq, uint8_t* marking);

// called by limitedFlood to perform marking of the flooded region
extern void limitedRecurse 
    (int32_t width, int32_t height, const uint8_t* inRed, 
     const uint8_t* inGreen, const uint8_t* inBlue, int32_t origX, 
     int32_t origY, int32_t x, int32_t y, uint32_t distSq, uint8_t* marking);

// ------------------------------------------------------------------------

#endif // MP8_H
