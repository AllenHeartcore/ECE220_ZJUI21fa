/*	###### Copyright Information ######
	This is the program (2nd part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem VIII. 
	Written, debugged and committed by Chen, Ziyuan on 14 November 2021. 
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp8 <input_file> <output_file> <command_#> <x> <y> <RRGGBB> [<distSq>]
	The program outputs:	A .txt or .png file named <output_file>

	###### Function Documentation ######
	The commands function as follows: 
		No.	Command		Functions involved
		1	Basic flood	basicFlood*, basicRecurse
		2	Grey flood	greyFlood*, greyRecurse, colorsWithinDistSq*
		3	Limited flood	limitedFlood*, limitedRecurse, colorsWithinDistSq*
		[Note]  Functions labelled with "*" is defined in the (1st part). 

	The functions operate as follows: 
		Function		Parameters							Outputs		Return value
		basicRecurse		(w, h, inR, inG, inB, X, Y, marking)				(marking)	NONE
		greyRecurse		(w, h, inR, inG, inB, X, Y, distSq, marking)			(marking)	NONE
		limitedRecurse		(w, h, inR, inG, inB, origX, origY, X, Y, distSq, marking)	(marking)	NONE

		Function		Functionality
		basicRecurse		Recursive floodFill from a point until reaching white pixels or the image border
		greyRecurse		Recursive floodFill from a point until reaching near-white pixels or the image border
		limitedRecurse		Recursive floodFill from a point until reaching pixels too different (in RGB) \
						from the color at the starting point, too far away (> 35 pixels), or beyond the image border

		[Variable explanation]
			w, h				-- width and height of the input image
			origX, origY			-- starting position of the flood
			X, Y				-- current position of the flood
			inR, inG, inB			-- pointers to color channels (1-D arrays of size h*w)
			distSq				-- (in greyRecurse) max distance squared between white and boundary pixel color
							-- (in limitedRecurse) max distance squared between pixel at origin and boundary pixel color
			marking				-- Array used to mark seen positions (indexed in the same way as the RGB input arrays)
		[Note]  No side effect claimed, yet this in no case serves as a guarantee. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp8.h"
#define CWDS colorsWithinDistSq
#define SQUARE(x) (x) * (x)

void basicRecurse (int32_t width, int32_t height, const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
		int32_t x, int32_t y, uint8_t* marking) {
	if (marking[y * width + x] || (inRed[y * width + x] & inGreen[y * width + x] & inBlue[y * width + x]) == 255) return;
	marking[y * width + x] = 1;								// Return for marked and white pixels
	if (y > 0) basicRecurse(width, height, inRed, inGreen, inBlue, x, y - 1, marking);			// Search up
	if (x < width - 1) basicRecurse(width, height, inRed, inGreen, inBlue, x + 1, y, marking);		// Search right
	if (y < height - 1) basicRecurse(width, height, inRed, inGreen, inBlue, x, y + 1, marking);		// Search down
	if (x > 0) basicRecurse(width, height, inRed, inGreen, inBlue, x - 1, y, marking);			// Search left
}

void greyRecurse (int32_t width, int32_t height, const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
		int32_t x, int32_t y, uint32_t distSq, uint8_t* marking) {
	if (marking[y * width + x] || CWDS(inRed[y * width + x], inGreen[y * width + x], inBlue[y * width + x], 255, 255, 255, distSq)) return;
	marking[y * width + x] = 1;								// Return for marked and grey pixels
	if (y > 0) greyRecurse(width, height, inRed, inGreen, inBlue, x, y - 1, distSq, marking);
	if (x < width - 1) greyRecurse(width, height, inRed, inGreen, inBlue, x + 1, y, distSq, marking);
	if (y < height - 1) greyRecurse(width, height, inRed, inGreen, inBlue, x, y + 1, distSq, marking);
	if (x > 0) greyRecurse(width, height, inRed, inGreen, inBlue, x - 1, y, distSq, marking);
}

void limitedRecurse (int32_t width, int32_t height, const uint8_t* inRed, const uint8_t* inGreen, const uint8_t* inBlue, 
		int32_t origX, int32_t origY, int32_t x, int32_t y, uint32_t distSq, uint8_t* marking) {
	if ((SQUARE(x - origX) + SQUARE(y - origY) > 1225) || marking[y * width + x] || !CWDS(inRed[y * width + x], inGreen[y * width + x], 
		inBlue[y * width + x], inRed[origY * width + origX], inGreen[origY * width + origX], inBlue[origY * width + origX], distSq)) return;
	marking[y * width + x] = 1;								// Return for distant, marked and dissimilar pixels
	if (y > 0) limitedRecurse(width, height, inRed, inGreen, inBlue, origX, origY, x, y - 1, distSq, marking);
	if (x < width - 1) limitedRecurse(width, height, inRed, inGreen, inBlue, origX, origY, x + 1, y, distSq, marking);
	if (y < height - 1) limitedRecurse(width, height, inRed, inGreen, inBlue, origX, origY, x, y + 1, distSq, marking);
	if (x > 0) limitedRecurse(width, height, inRed, inGreen, inBlue, origX, origY, x - 1, y, distSq, marking);
}
