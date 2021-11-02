#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "lab7.h"

void 
edgeDetect (int32_t width, int32_t height, 
	    const int32_t* sobelX, const int32_t* sobelY,
	    const uint8_t* inRed, const uint8_t* inGreen, 
	    const uint8_t* inBlue, uint8_t* outMono, uint8_t* temp)
{
	for (int pixel = 0; pixel < height * width; pixel++) temp[pixel] = 0.2125 * inRed[pixel] + 0.7154 * inGreen[pixel] + 0.0721 * inBlue[pixel];

	int32_t Gx = 0, Gy = 0;
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			Gx = 0; Gy = 0;
			Gx += temp[(y - 1) * width + x - 1] * sobelX[0] + temp[(y - 1) * width + x] * sobelX[1] + temp[(y - 1) * width + x + 1] * sobelX[2];
			Gx += temp[y * width + x - 1] * sobelX[3] + temp[y * width + x] * sobelX[4] + temp[y * width + x + 1] * sobelX[5];
			Gx += temp[(y + 1) * width + x - 1] * sobelX[6] + temp[(y + 1) * width + x] * sobelX[7] + temp[(y + 1) * width + x + 1] * sobelX[8];
			Gy += temp[(y - 1) * width + x - 1] * sobelY[0] + temp[(y - 1) * width + x] * sobelY[1] + temp[(y - 1) * width + x + 1] * sobelY[2];
			Gy += temp[y * width + x - 1] * sobelY[3] + temp[y * width + x] * sobelY[4] + temp[y * width + x + 1] * sobelY[5];
			Gy += temp[(y + 1) * width + x - 1] * sobelY[6] + temp[(y + 1) * width + x] * sobelY[7] + temp[(y + 1) * width + x + 1] * sobelY[8];
			outMono[y * width + x] = sqrt(Gx * Gx + Gy * Gy);
			if (outMono[y * width + x] > 255) outMono[y * width + x] = 255;
		}
	}
}

