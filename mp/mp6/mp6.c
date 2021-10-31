/* mp6.c - skeleton source file for ECE220 image processing MP6
 *
 * "Copyright (c) 2021 by Steven S. Lumetta"
 *
 * Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without written agreement
 * is hereby granted, provided that the above copyright notice and the following two paragraphs appear in all copies of this software. 
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 *
 * Author:	    	Steven S. Lumetta
 * Version:	    	1
 * Creation Date:   	19 July 2021
 * Filename:	    	mp6.c
 * History:
 *			SSL	1	19 July 2021		First written. 
 */

/*  	###### Copyright Information ######
	This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem VI. 
	Written and commented by Chen, Ziyuan on 29 October 2021. 
	Debugged and committed by Chen, Ziyuan on 30 October 2021. 
	Refined and re-committed by Chen, Ziyuan on 31 October 2021. 
	Copyright © 2021 Steven S. Lumetta and Ziyuan Chen. 

	###### Functionality ######
	Compile by running:	make clean
	Execute by running:	./mp6 <PNG_input_file> <output_file> <transform_no.>
	The program outputs:	A .txt or .png file named <output_file>

	###### Function Documentation ######
	The transforms function as follows: 
		No. 	Transform			Conversions involved
		0	Writes human-readable HSL	convert_RGB_to_HSL
		1	RGB <-> HSL conversion		convert_RGB_to_HSL, convert_HSL_to_RGB
		2	Edge detection			compute_sober_kernals
		3	Edge sharpening			compute_sober_kernals
		4	All plus equalization		compute_sober_kernals, equalize_intensities

	The conversions function as follows: 
		Conversion		Parameters			Outputs		Functionality
		convert_RGB_to_HSL	(height, width, R, G, B)	(H, S, L)	Convert RGB image data into HSL channel data. 
		convert_HSL_to_RGB	(height, width, H, S, L)	(R, G, B)	Convert HSL channel data into RGB image data. 
		compute_sobel_kernels	(height, width, L)		(Gx, Gy)	Compute X and Y Sobel edge detection kernels using convolution. 
		equalize_intensities	(height, width, L)		(L_modified)	Perform histogram equalization to re-map image intensities.

	Notes: 	Ranges of values are shown on the right -> R(0~255), G(0~255), B(0~255), H(0~49151), S(0~32768), L(0~510). 
		All RGB data, HSL data, and Sobel kernal pixels are arranged in column-major order. 
		All functions return no value. 
		In the "compute_sobel_kernals" function, edge pixels may be ignored, and kernel outputs for these pixels left undefined.
		Aside from this function, no side effect is claimed, yet this in no case serves as a guarantee. 
*/

#include <stdio.h>
#include <stdlib.h>
#include "mp6.h"
#define BIMAX(x, y) (x > y ? x : y)
#define TRIMAX(x, y, z) (BIMAX(x, y) > z ? BIMAX(x, y) : z)
#define BIMIN(x, y) (x < y ? x : y)
#define TRIMIN(x, y, z) (BIMIN(x, y) < z ? BIMIN(x, y) : z)

void convert_RGB_to_HSL (int32_t height, int32_t width, const uint8_t* red, const uint8_t* green, const uint8_t* blue, 
			uint16_t* H, uint16_t* S, uint16_t* L) {
	int32_t R, G, B, M, N, C, h, s, l;
	for (int32_t pixel = 0; pixel < height * width; pixel++) {
		R = red[pixel];								// Import RGB
		G = green[pixel];
		B = blue[pixel];
		M = TRIMAX(R, G, B);							// Calculate intermediate values
		N = TRIMIN(R, G, B);
		l = M + N;
		C = M - N;
		if (M == 0 || N == 255) s = 0;						// Judging conds. for S
		else if (l > 0 && l <= 255) s = 0x8000 * C / l;
		else s = 0x8000 * C / (510 - l);
		if (C == 0) h = 0;							// Judging conds. for H
		else if (C > 0 && M == R) h = 0x2000 * (C + G - B) / C;
		else if (C > 0 && M > R && M == G) h = 0x2000 * (3 * C + B - R) / C;
		else h = 0x2000 * (5 * C + R - G) / C;
		H[pixel] = h;								// Export HSL
		S[pixel] = s;
		L[pixel] = l;
	}
}

void convert_HSL_to_RGB (int32_t height, int32_t width, const uint16_t* H, const uint16_t* S, const uint16_t* L, 
			uint8_t* red, uint8_t* green, uint8_t* blue) {
	int32_t R, G, B, M, N, T, C, h, s, l, h_major, h_minor;
	for (int32_t pixel = 0; pixel < height * width; pixel++) {
		h = H[pixel];								// Import HSL
		s = S[pixel];
		l = L[pixel];
		if (l <= 255) C = s * l / 0x8000;					// Calc. interm. values
		else C = s * (510 - l) / 0x8000;
		N = (l - C) / 2;							// Calc. MN
		M = N + C;
		h_major = h / 0x2000;							// Judging conds. for T
		h_minor = h & 0x3FFF;
		if (h_minor >= 0x2000) T = N + C * (h_minor - 0x2000) / 0x2000;
		else T = N + C * (0x2000 - h_minor) / 0x2000;
		switch (h_major) {							// Judging conds. for MNT->RGB mapping
			case 0:  R = M; G = N; B = T; break;
			case 1:  R = M; G = T; B = N; break;
			case 2:  R = T; G = M; B = N; break;
			case 3:  R = N; G = M; B = T; break;
			case 4:  R = N; G = T; B = M; break;
			default: R = T; G = N; B = M; break;
		}
		red[pixel] = R;								// Export RGB
		green[pixel] = G;
		blue[pixel] = B;
	}
}

void compute_sobel_kernels (int32_t height, int32_t width, const uint16_t* L, 
			int32_t* Gx, int32_t* Gy) {
	int32_t x_kernal, y_kernal;
	for (int32_t x = 1; x < height - 1; x++) {
		for (int32_t y = 1; y < width - 1; y++) {
			x_kernal = (L[x * width + y - 1] - L[x * width + y + 1]) * 2;			// Calc. x_kernal
			x_kernal += (L[(x - 1) * width + y - 1] - L[(x - 1) * width + y + 1]);
			x_kernal += (L[(x + 1) * width + y - 1] - L[(x + 1) * width + y + 1]);
			y_kernal = (L[(x - 1) * width + y] - L[(x + 1) * width + y]) * 2;		// Calc. y_kernal
			y_kernal += (L[(x - 1) * width + y - 1] - L[(x + 1) * width + y - 1]);
			y_kernal += (L[(x - 1) * width + y + 1] - L[(x + 1) * width + y + 1]);
			Gx[x * width + y] = x_kernal;
			Gy[x * width + y] = y_kernal;

		}
	}
}

void equalize_intensities (int32_t height, int32_t width, 
			uint16_t* L) {
	int64_t H[512], K[512], M[512], size = height * width;
	for (uint16_t init = 0; init <= 510; init++) H[init] = 0;			// Initialize H (histogram)
	for (int32_t pixel = 0; pixel < size; pixel++) H[L[pixel]] += 1;		// Build H
	K[0] = H[0];									// Initialize K (accumulated histogram)
	M[0] = (511 * K[0] + size - 1) / size - 1;					// Initialize M (mapped histogram)
	for (uint16_t iter = 1; iter <= 510; iter++) {
		K[iter] = K[iter - 1] + H[iter];					// Build K
		M[iter] = (511 * K[iter] + size - 1) / size - 1;			// Build M
	}
	for (int32_t pixel = 0; pixel < size; pixel++) L[pixel] = M[L[pixel]];
}