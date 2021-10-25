#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "imageData.h"
#include "lab7.h"

int
main (int argc, const char* const argv[])
{
    int32_t sobelX[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
    int32_t sobelY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

    if (3 != argc) {
        fprintf (stderr, "syntax: %s <input PNG> <output PNG>\n", argv[0]);
	return 2;
    }

    Image* inputImage = decode (argv[1]);
    printf ("Width: %d, height: %d\n", inputImage->width, inputImage->height);

    Image* outputImage = generateOutput (inputImage);

    int height = inputImage->height;
    int width  = inputImage->width;
    uint8_t* inRed    = inputImage->redChannel;
    uint8_t* inBlue   = inputImage->blueChannel;
    uint8_t* inGreen  = inputImage->greenChannel;
    uint8_t* inAlpha  = inputImage->alphaChannel;
    uint8_t* outRed   = outputImage->redChannel;
    uint8_t* outBlue  = outputImage->blueChannel;
    uint8_t* outGreen = outputImage->greenChannel;
    uint8_t* outAlpha = outputImage->alphaChannel;

    memcpy (outAlpha, inAlpha, sizeof (*outAlpha) * height * width);
    edgeDetect (width, height, sobelX, sobelY, inRed, inGreen, inBlue, 
    		outRed, outGreen);
    memcpy (outGreen, outRed, sizeof (*outRed) * height * width);
    memcpy (outBlue, outRed, sizeof (*outRed) * height * width);

    encode (argv[2], outputImage);

    freeImage (inputImage);
    freeImage (outputImage);

    return 0;
}
