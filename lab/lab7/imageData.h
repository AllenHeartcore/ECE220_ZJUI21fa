#if !defined(IMAGEDATA_H)
#define IMAGEDATA_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "lodepng.h"

typedef struct
{
  int width;
  int height;
  uint8_t* redChannel;
  uint8_t* blueChannel;
  uint8_t* greenChannel;
  uint8_t* alphaChannel;
} Image;

extern void encode (const char* filename, Image* encodeImage);
extern Image* decode (const char* filename);
extern Image* generateOutput (Image* inputImage);
extern void freeImage (Image* inputImage);

#endif // IMAGEDATA_H
