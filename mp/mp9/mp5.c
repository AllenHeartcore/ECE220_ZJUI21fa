/* mp5.c - source file for ECE220 picture drawing program
 *
 * "Copyright (c) 2018 by Charles H. Zega, and Saransh Sinha."
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
 * Author:		Charles Zega, Saransh Sinha
 * Version:		1
 * Creation Date:	12 February 2018
 * Filename:		mp5.c
 * History:
 *			CZ	1	12 February 2018	First written. 
 */

/*  ###### Copyright Information ######
	This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem V. 
	Written and commented by Chen, Ziyuan on 22 October 2021. 
	Debugged and committed by Chen, Ziyuan on 23 October 2021. 
	Refined and re-committed by Chen, Ziyuan on 26 October 2021. 
	Copyright © 2021 Charles Zega, Saransh Sinha, and Ziyuan Chen. 

	###### Functionality ######
	Compile by running:		make clean
	Execute by running:		./mp5 <type>
	The program outputs:	A png image file named "out%d.png"%<type>. 
	The types function as follows:
		Type		Function		Parameters				Functionality
		1		near_horizontal		(x_start, y_start, x_end, y_end)	Draws a line between two pixels (including endpoints). 
		2		near_vertical		(x_start, y_start, x_end, y_end)	Draws a line between two pixels (including endpoints). 
		3		draw_line		(x_start, y_start, x_end, y_end)	Draws a line between two pixels (including endpoints). 
		4		draw_rect		(x_A, y_A, width, height)		Draws edges of the rectangle ABCD. (BCD coords. inferred)
		5		draw_triangle		(x_A, y_A, x_B, y_B, x_C, y_C)		Draws edges of the triangle ABC. 
		6		draw_parallelogram	(x_A, y_A, x_B, y_B, x_C, y_C)		Draws edges of the parallelogram ABCD. (D coord. inferred)
		7		draw_circle		(x_cntr, y_cntr, r_inner, r_outer)	Draws a circular ring. 
		8		rect_gradient		(x_start, y_start, width, height, start_clr, end_clr)	Fills a rect. with a horizontal gradient. 
		default		draw_picture		None					Draws a customized image by calling other functions. 
	Note: 	All functions return 0 if any of the pixels drawn are out of bounds, otherwise 1. 
			No side effect claimed, yet this in no case serves as a guarantee. 
*/

#include <stdio.h>
#include <stdlib.h>
#include "mp5.h"
#define MASK_R 0x00FF0000
#define MASK_G 0x0000FF00
#define MASK_B 0x000000FF
#define SGN(x) (x > 0 ? 1 : (x == 0 ? 0 : -1))
#define SQUARE(x) (x) * (x)									// Parentheses are necessary

int32_t near_horizontal (int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end) {
	float x_1 = (float) x_start;								// Deal with coords. in float to avoid int division
	float y_1 = (float) y_start;
	float x_2 = (float) x_end;
	float y_2 = (float) y_end;
	uint8_t return_val = 1;									// Track the return value
	if (x_start <= x_end) {
		for (int32_t x = x_start; x <= x_end; x++) {
			float y = (y_2 - y_1) * ((float) x - x_1) / (x_2 - x_1) + y_1;
			return_val &= draw_dot(x, (int32_t) (y + 0.5));				// Rounding-up coords.
		}
	} else {
		for (int32_t x = x_start; x >= x_end; x--) {
			float y = (y_2 - y_1) * ((float) x - x_1) / (x_2 - x_1) + y_1;
			return_val &= draw_dot(x, (int32_t) (y + 0.5));
		}
	}
	return return_val;
}

int32_t near_vertical (int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end) {
	float x_1 = (float) x_start;
	float y_1 = (float) y_start;
	float x_2 = (float) x_end;
	float y_2 = (float) y_end;
	uint8_t return_val = 1;
	if (y_start == y_end) {
		return_val &= draw_dot(x_start, y_start);					// The "single-dot" case
		return return_val;
	} else {
		if (y_start < y_end) {
			for (int32_t y = y_start; y <= y_end; y++) {
				float x = (x_2 - x_1) * ((float) y - y_1) / (y_2 - y_1) + x_1;
				return_val &= draw_dot((int32_t) (x + 0.5), y);
			}
		} else {
			for (int32_t y = y_start; y >= y_end; y--) {
				float x = (x_2 - x_1) * ((float) y - y_1) / (y_2 - y_1) + x_1;
				return_val &= draw_dot((int32_t) (x + 0.5), y);
			}
		}
	}
	return return_val;
}

int32_t draw_line (int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end) {
	float x_1 = (float) x_start;
	float y_1 = (float) y_start;
	float x_2 = (float) x_end;
	float y_2 = (float) y_end;
	uint8_t return_val = 1;
	if (x_start == x_end) return_val = near_vertical(x_start, y_start, x_end, y_end);	// The "vertical" case
	else {
		float slope = (y_2 - y_1) / (x_2 - x_1);
		if ((slope >= -1.0) && (slope <= 1.0)) return_val = near_horizontal(x_start, y_start, x_end, y_end);
		else return_val = near_vertical(x_start, y_start, x_end, y_end);
	}
	return return_val;
}

int32_t draw_rect (int32_t x, int32_t y, int32_t w, int32_t h) {
	if ((w < 0) || (h < 0)) return 0;							// The "negative w/h" case
	uint8_t return_val = 1;
	return_val &= draw_line(x, y, x + w, y);
	return_val &= draw_line(x, y, x, y + h);
	return_val &= draw_line(x + w, y, x + w, y + h);
	return_val &= draw_line(x, y + h, x + w, y + h);
	return return_val;
}

int32_t draw_triangle (int32_t x_A, int32_t y_A, int32_t x_B, int32_t y_B, int32_t x_C, int32_t y_C) {
	uint8_t return_val = 1;
	return_val &= draw_line(x_A, y_A, x_B, y_B);
	return_val &= draw_line(x_A, y_A, x_C, y_C);
	return_val &= draw_line(x_B, y_B, x_C, y_C);
	return return_val;
}

int32_t draw_parallelogram (int32_t x_A, int32_t y_A, int32_t x_B, int32_t y_B, int32_t x_C, int32_t y_C) {
	int32_t x_D = x_A - x_B + x_C, y_D = y_A - y_B + y_C;					// Point D's coord. inferred
	uint8_t return_val = 1;
	return_val &= draw_line(x_A, y_A, x_B, y_B);
	return_val &= draw_line(x_B, y_B, x_C, y_C);
	return_val &= draw_line(x_C, y_C, x_D, y_D);
	return_val &= draw_line(x_D, y_D, x_A, y_A);
	return return_val;
}

int32_t draw_circle (int32_t x, int32_t y, int32_t inner_r, int32_t outer_r) {
	if ((inner_r < 0) || (outer_r < inner_r)) return 0;					// The "invalid radii" case
	uint8_t return_val = 1;
	for (int32_t x_coord = x - outer_r; x_coord <= x + outer_r; x_coord++) {
		for (int32_t y_coord = y - outer_r; y_coord <= y + outer_r; y_coord++) {
			int32_t radius_square = SQUARE(x_coord - x) + SQUARE(y_coord - y);
			if ((radius_square >= SQUARE(inner_r)) && (radius_square <= SQUARE(outer_r))) return_val &= draw_dot(x_coord, y_coord);
		}
	}
	return return_val;
}

int32_t rect_gradient (int32_t x, int32_t y, int32_t w, int32_t h, int32_t start_color, int32_t end_color) {
	if ((w < 1) || (h < 0)) return 0;
	uint8_t start_R = (start_color & MASK_R) >> 16;						// Unpack color code
	uint8_t start_G = (start_color & MASK_G) >> 8;
	uint8_t start_B = (start_color & MASK_B);
	uint8_t end_R = (end_color & MASK_R) >> 16;
	uint8_t end_G = (end_color & MASK_G) >> 8;
	uint8_t end_B = (end_color & MASK_B);
	uint8_t return_val = 1;
	for (int32_t x_coord = x; x_coord <= x + w; x_coord++) {
		for (int32_t y_coord = y; y_coord <= y + h; y_coord++) {
			uint8_t coord_R = (2 * (x_coord - x) * (end_R - start_R) + w * SGN(end_R - start_R)) / (2 * w) + start_R;
			uint8_t coord_G = (2 * (x_coord - x) * (end_G - start_G) + w * SGN(end_G - start_G)) / (2 * w) + start_G;
			uint8_t coord_B = (2 * (x_coord - x) * (end_B - start_B) + w * SGN(end_B - start_B)) / (2 * w) + start_B;
			int32_t coord_color = (coord_R << 16) | (coord_G << 8) | (coord_B);	// Repack color code
			set_color(coord_color);
			return_val &= draw_dot(x_coord, y_coord);
		}
	}
	return return_val;
}

int32_t draw_picture () {
	return 0;
}