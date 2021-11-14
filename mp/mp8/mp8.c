/*	###### Copyright Information ######
	This is the program (1st part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem VIII. 
	Written, debugged and committed by Chen, Ziyuan on 14 November 2021. 
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp8 <input_file> <output_file> <command_#> <x> <y> <RRGGBB> [<distSq>]
	The program outputs:	A .txt or .png file named <output_file>

	###### Function Documentation ######
	The commands function as follows: 
		No.	Command		Functions involved
		1	Basic flood	basicFlood, basicRecurse*
		2	Grey flood	greyFlood, greyRecurse*, colorsWithinDistSq
		3	Limited flood	limitedFlood, limitedRecurse*, colorsWithinDistSq
		[Note]  Functions labelled with "*" is defined in the (2nd part). 

	The functions operate as follows: 
		Function		Parameters						Outputs			Return value
		basicFlood		(w, h, inR, inG, inB, X, Y, fldR, fldG, fldB)		(outR, outG, outB)	NONE
		greyFlood		(w, h, inR, inG, inB, X, Y, fldR, fldG, fldB, distSq)	(outR, outG, outB)	NONE
		limitedFlood		(w, h, inR, inG, inB, X, Y, fldR, fldG, fldB, distSq)	(outR, outG, outB)	NONE
		colorsWithinDistSq	(r1, g1, b1, r2, g2, b2, distSq)			NONE	sum(square(diff(components))) <= distSq ? 1 : 0

		Function		Functionality
		basicFlood		Wrapper for recursive floodFill from a point until reaching white pixels or the image border
		greyFlood		Wrapper for recursive floodFill from a point until reaching near-white pixels or the image border
		limitedFlood		Wrapper for recursive floodFill from a point until reaching pixels too different (in RGB) \
						from the color at the starting point, too far away (> 35 pixels), or beyond the image border
		colorsWithinDistSq	Determines if two colors are within Euclidean distance squared of one another in RGB space

		[Variable explanation]
			w, h				-- width and height of the input image
			X, Y				-- starting position of the flood
			inR, inG, inB, outR, outG, outB	-- pointers to color channels (1-D arrays of size h*w)
			fldR, fldG, fldB		-- color components of the flooding color
			distSq				-- (in colorsWithinDistSq) max distance squared for check
							-- (in greyFlood) max distance squared between white and boundary pixel color
							-- (in limitedFlood) max distance squared between pixel at origin and boundary pixel color
		[Note]  No side effect claimed, yet this in no case serves as a guarantee. 
*/

#include <stdlib.h>
#include <string.h>
#include "mp8.h"
#define SQUARE(x) (x) * (x)

void basicFlood (int32_t width, int32_t height, const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
		int32_t startX, int32_t startY, uint8_t floodR, uint8_t floodG, uint8_t floodB, 
		uint8_t* outRed, uint8_t* outGreen, uint8_t* outBlue) {
	memset(outRed, 0, width * height);									// Init marking array
	basicRecurse(width, height, inRed, inGreen, inBlue, startX, startY, outRed);				// Run basic recurse
	for (int64_t h_iter = 0; h_iter < height; h_iter++)
		for (int64_t w_iter = 0; w_iter < width; w_iter++)
			if (outRed[h_iter * width + w_iter]) {							// Fill in the flooding color
				outRed[h_iter * width + w_iter] = floodR;
				outGreen[h_iter * width + w_iter] = floodG;
				outBlue[h_iter * width + w_iter] = floodB;
			} else {										// Fill in the original color
				outRed[h_iter * width + w_iter] = inRed[h_iter * width + w_iter];
				outGreen[h_iter * width + w_iter] = inGreen[h_iter * width + w_iter];
				outBlue[h_iter * width + w_iter] = inBlue[h_iter * width + w_iter];
			}
}

int32_t colorsWithinDistSq (uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, uint32_t distSq) {
	if (SQUARE(r1 - r2) + SQUARE(g1 - g2) + SQUARE(b1 - b2) <= distSq) return 1;
	else return 0;
}

void greyFlood (int32_t width, int32_t height, const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
		int32_t startX, int32_t startY, uint8_t floodR, uint8_t floodG, uint8_t floodB, uint32_t distSq, 
		uint8_t* outRed, uint8_t* outGreen, uint8_t* outBlue) {
	memset(outRed, 0, width * height);
	greyRecurse(width, height, inRed, inGreen, inBlue, startX, startY, distSq, outRed);			// Run grey recurse (with distSq)
	for (int64_t h_iter = 0; h_iter < height; h_iter++)
		for (int64_t w_iter = 0; w_iter < width; w_iter++)
			if (outRed[h_iter * width + w_iter]) {
				outRed[h_iter * width + w_iter] = floodR;
				outGreen[h_iter * width + w_iter] = floodG;
				outBlue[h_iter * width + w_iter] = floodB;
			} else {
				outRed[h_iter * width + w_iter] = inRed[h_iter * width + w_iter];
				outGreen[h_iter * width + w_iter] = inGreen[h_iter * width + w_iter];
				outBlue[h_iter * width + w_iter] = inBlue[h_iter * width + w_iter];
			}
}

void limitedFlood (int32_t width, int32_t height, const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
		int32_t startX, int32_t startY, uint8_t floodR, uint8_t floodG, uint8_t floodB, uint32_t distSq, 
		uint8_t* outRed, uint8_t* outGreen, uint8_t* outBlue) {
	memset(outRed, 0, width * height);
	limitedRecurse(width, height, inRed, inGreen, inBlue, startX, startY, startX, startY, distSq, outRed);	// Run limited recurse (with X, Y backed up)
	for (int64_t h_iter = 0; h_iter < height; h_iter++)
		for (int64_t w_iter = 0; w_iter < width; w_iter++)
			if (outRed[h_iter * width + w_iter]) {
				outRed[h_iter * width + w_iter] = floodR;
				outGreen[h_iter * width + w_iter] = floodG;
				outBlue[h_iter * width + w_iter] = floodB;
			} else {
				outRed[h_iter * width + w_iter] = inRed[h_iter * width + w_iter];
				outGreen[h_iter * width + w_iter] = inGreen[h_iter * width + w_iter];
				outBlue[h_iter * width + w_iter] = inBlue[h_iter * width + w_iter];
			}
}
