/*									tab:8
 *
 * mp5.h - header file for ECE220 picture drawing program
 *
 * "Copyright (c) 2018 by Charles H. Zega, and Saransh Sinha."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Charles Zega, Saransh Sinha
 * Version:	    2
 * Creation Date:   12 February 2018
 * Filename:	    mp5.h
 * History:
 *	CZ	1	12 February 2018
 *		First written.
 *	SL	2	13 February 2018
 *		Split into separate C files for compilation.
 */

#if !defined(MP5_H)
#define MP5_H

#include <stdint.h>



/***************************************************************************
 * 	You are required to implement the following functions in mp5.c!    *
 ***************************************************************************/

/*
 * Draws a line that has a slope between -1 and 1, inclusive. Returns 0 if
 * the line is out of bounds, 1 for success.
 */
extern int32_t near_horizontal(int32_t x_start, int32_t y_start, 
			       int32_t x_end, int32_t y_end);

/*
 * Draws a line that has a slope between -infinity and -1 or 1 and infinity,
 * excluding -1 and 1. Returns 0 if  the line is out of bounds, 1 for success.
 */
extern int32_t near_vertical(int32_t x_start, int32_t y_start, 
			     int32_t x_end, int32_t y_end);

/*
 * Draws any given line from (x_start,y_start) to (x_end,y_end). Calls
 * near_horizontal or near_vertical depending on the slope of the line.
 * Returns 0 if  the line is out of bounds, 1 for success.
 */
extern int32_t draw_line(int32_t x_start, int32_t y_start, 
		         int32_t x_end, int32_t y_end);

/*
 * Draws the border of a rectangle with a width given by width and a height
 * given by height. The top-left corner of the rectangle is (x,y). Uses
 * draw_line to draw the rectangle. Returns 0 if the line is out of bounds, 1
 * for success.
 */
extern int32_t draw_rect(int32_t x, int32_t y, int32_t w, int32_t h);

/*
 * Draws the edges of a triangle with vertices at (x_A,y_A),(x_B,y_B), and
 * (x_C,y_C). Uses draw_line to draw the edges. Returns 0 if any line is out
 * of bounds, 1 for success.
 */
extern int32_t draw_triangle(int32_t x_A, int32_t y_A, 
			     int32_t x_B, int32_t y_B, 
			     int32_t x_C, int32_t y_C);

/*
 * Draws the edges of a parallelogram with vertices at (x_A,y_A), (x_B,y_B),
 * and (x_C,y_C). The fourth vertex is at whichever location will form a
 * legal parallelogram. Uses draw_line to draw the edges. Returns 0 if the
 * line is out of bounds, 1 for success.
 */
extern int32_t draw_parallelogram(int32_t x_A, int32_t y_A, 
				  int32_t x_B, int32_t y_B, 
				  int32_t x_C, int32_t y_C);

/*
 * Draws a circle with a center at (x,y), an inner radius of inner_r, and
 * an outer radius of inner_r. It fills all pixels with distance from the
 * center that is greater than or equal to inner_r and less than or equal
 * to outer_r. Returns 0 if any of the circle is out of bounds, 1 for success.
 */
extern int32_t draw_circle(int32_t x, int32_t y, 
			   int32_t inner_r, int32_t outer_r);

/*
 * Draws a filled rectangle with top-left corner at (x,y), width of width,
 * and height of height. The color of each pixel is based on it relative
 * distance between the left and right edges of the rectangle and the
 * the difference for each channel between end_color and start_color.
 * Returns 0 if any of the rectangle is out of bounds, 1 for success.
 */
extern int32_t rect_gradient(int32_t x, int32_t y, int32_t w, int32_t h, 
			     int32_t start_color, int32_t end_color);

/*
 * Draws any picture the student would like. This is an exercise to practice
 * calling functions. The image drawn must be of a complexity specified in
 * the documentation. Returns 0 if any pixel drawn is out of bounds, 1 for
 * success.
 */
extern int32_t draw_picture();

// added for lab 9
extern void draw_tree (void);

/************************************************************************
 * PREPROCESSOR CONSTANTS                                               *
 ************************************************************************/

/* Do NOT modify anything below this line. */

#define WIDTH 624	//Definitions for the size of the PNG picture.
#define HEIGHT 320


/************************************************************************
 * THE ROUTINES IN THIS SECTION ARE PROVIDED FOR YOUR USE (in main.c).  *
 ************************************************************************/

/* 
 *  set_color
 *	 
 *	 
 *	
 *	
 * INPUTS: new_color represents the color for future calls to draw_dot where:
 * bits 31-24 are ignored
 * bits 23-16 are red
 * bits 15-8 are green
 * bits 7-0 are blue
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: sets color to be new_color
 *
 *
 * Changes color of pixels drawn using draw_dot. The format of the input
 * is the most significant byte being ignored, the second most significant
 * byte representing red, the third green, and the least significant byte
 * being blue.
 */
extern void set_color(int32_t new_color);

/* 
 * draw_dot
 *	 
 *	 
 *	
 *	
 * INPUTS: x,y -- the coordinates of a new pixel to be drawn
 * OUTPUTS: sets the element in the picture_data array that represents the coordinates
 * (x,y) to be a pixel_t that represents the color given by the variable, color
 * RETURN VALUE: 1 if the pixel is in bounds, 0 if it is out of bounds
 * SIDE EFFECTS: none
 *
 *
 * Draws a pixel at the location (x,y) of the color set by the most recent
 * call to set color. Returns 1 if the pixel is in bounds and 0 if the pixel
 * is out of bounds. Does not draw anything if it is out of bounds.
 */
extern int32_t draw_dot(int32_t x, int32_t y);




#endif // MP5_H
