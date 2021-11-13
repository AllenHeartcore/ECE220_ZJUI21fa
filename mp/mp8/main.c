#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "imageData.h"
#include "mp8.h"

int
main (int argc, const char* const argv[])
{
    char garbage[2];
    int command;
    char* readVal;
    long int RGB;
    int startX;
    int startY;
    uint8_t floodR;
    uint8_t floodG;
    uint8_t floodB;
    int distSq;

    if (7 > argc || 8 < argc ||
        1 != sscanf (argv[3], "%d%1s", &command, garbage) || 
        1 > command || 3 < command || 
	(8 != argc && (2 == command || 3 == command))) {
        fprintf (stderr, 
	         "syntax: %s <input file> <output file> <command #> ...\n", 
	         argv[0]);
	fprintf (stderr, "\nCommand must be from 1 to 3.\n");
	fprintf (stderr, "  1 -- flood to black <x> <y> <RGB>\n");
	fprintf (stderr, "  2 -- flood to grey <x> <y> <RGB> <distSq>\n");
	fprintf (stderr, "  3 -- flood to different <x> <y> <RGB> <distSq>\n");
	return 2;
    }

    startX = strtol (argv[4], &readVal, 10);
    if ('\0' != *readVal || 0 > startX) {
	fprintf (stderr, "x must be a non-negative decimal value.\n");
	return 2;
    }

    startY = strtol (argv[5], &readVal, 10);
    if ('\0' != *readVal || 0 > startY) {
	fprintf (stderr, "y must be a non-negative decimal value.\n");
	return 2;
    }

    RGB = strtol (argv[6], &readVal, 16);
    if ('\0' != *readVal) {
	fprintf (stderr, "RGB must be a hex value.\n");
	return 2;
    }
    floodR = ((RGB >> 16) & 0xFF);
    floodG = ((RGB >>  8) & 0xFF);
    floodB = ((RGB >>  0) & 0xFF);

    if (1 != command) {
	distSq = strtol (argv[7], &readVal, 10);
	if ('\0' != *readVal || 0 > distSq) {
	    fprintf (stderr, "distSq must be a non-negative decimal value.\n");
	    return 2;
	}
    }

    Image* inputImage = decode (argv[1]);
    printf ("Width: %d, height: %d\n", inputImage->width, inputImage->height);

    if (3 > command && 5000 < inputImage->height * inputImage->width) {
        fprintf (stderr, "Image too large for unbounded recursive flooding.\n");
	freeImage (inputImage);
	return 2;
    }
    if (inputImage->height <= startY || inputImage->width <= startX) {
        fprintf (stderr, "(x,y) must lie within the image.\n");
	freeImage (inputImage);
	return 2;
    }

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
    switch (command) {
	case 1:
	    basicFlood (width, height, inRed, inGreen, inBlue, 
			startX, startY, floodR, floodG, floodB,
			outRed, outGreen, outBlue);
	    break;
	case 2:
	    greyFlood (width, height, inRed, inGreen, inBlue, 
		       startX, startY, floodR, floodG, floodB, distSq,
		       outRed, outGreen, outBlue);
	    break;
	case 3:
	    limitedFlood (width, height, inRed, inGreen, inBlue, 
		          startX, startY, floodR, floodG, floodB, distSq,
		          outRed, outGreen, outBlue);
	    break;
    }
    encode (argv[2], outputImage);

    freeImage (inputImage);
    freeImage (outputImage);

    return 0;
}
