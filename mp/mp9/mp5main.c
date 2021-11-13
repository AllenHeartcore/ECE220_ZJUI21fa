/*									tab:8
 *
 * main.c - source file for ECE220 picture drawing program wrapper code
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
 *		Split into separate C files for compilation.  Made local
 *              functions and structures file scope.
 */

#include <png.h>
#include <stdlib.h>

#include "mp5.h"

/*
	A struct to hold pixel data, which is put into the picture_data array.
	The picture_data array in turn is used by makepng() to make your png!
*/

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} pixel_t;

static int32_t color = 0x00FFFFFF; // File-scope variable for the color of pixels, currently white.
static pixel_t * picture_data = NULL; // File-scope pointer to an array of pixels

/***************************************************************************
 * THE ROUTINES IN THIS SECTION ARE USED (in main.c) TO GENERATE THE IMAGE *
 ***************************************************************************/


static pixel_t int_to_pixel(int32_t integer);
static void make_png(char * str);
static void initialize_and_clear_canvas();
static void free_image_data();

int main(int argc, char * argv[]){

	initialize_and_clear_canvas(); 		//Initializes and clears canvas.
	if(argc < 2){
// changed for mp7
		draw_map(); 									// Call to student's function for image creation
		make_png("image.png");
	}
	else switch((argv[1])[0]){
		case '1' :
			set_color(0);
			near_horizontal(15,20,200,100);
			make_png("out1.png");
			break;
		case '2' :
			set_color(0);
			near_vertical(300,20,320,300);
			make_png("out2.png");
			break;
		case '3' :
			set_color(0);
			draw_line(15,20,200,100);
			draw_line(300,20,320,300);
			make_png("out3.png");
			break;
		case '4' :
			set_color(0);
			draw_triangle(15,20,500,100,300,300);
			make_png("out4.png");
			break;
		case '5' :
			set_color(0);
			draw_parallelogram(10,40,200,150,400,200);
			make_png("out5.png");
			break;
		case '6' :
			set_color(0);
			draw_rect(100,100,500,100);
			make_png("out6.png");
			break;
		case '7' :
			set_color(0);
			draw_circle(120,120,20,100);
			make_png("out7.png");
			break;
		case '8' :
			set_color(0);
			rect_gradient(100,100,500,100,0x00FF0000,0x0000FF00);
			make_png("out8.png");
			break;
		default :
			draw_map();
			make_png("image.png");
			break;
	}
	free_image_data();								// Free all the image data after the PNG is made.
	return 0;
}

/* 
 *  initialize_and_clear_canvas
 *	 
 *	 
 *	
 *	
 * INPUTS: none
 * OUTPUTS: sets picture_data to be an array representing the image
 * RETURN VALUE: none
 * SIDE EFFECTS: allocates a WIDTH * HEIGHT size buffer
 */


static void initialize_and_clear_canvas(){
	picture_data = malloc(WIDTH * HEIGHT * sizeof(pixel_t));
	int32_t i,j;
	// Make the whole thing white
	for(i = 0; i < HEIGHT; i++){
			for(j = 0; j < WIDTH; j++){
		picture_data[i*WIDTH + j] = int_to_pixel(color);
		}
	}


	return;
}

/* 
 *  free_image_data
 *	 
 *	 
 *	
 *	
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: frees the array at picture_data
 */

static void free_image_data(){
	free(picture_data);

	return;
}

/* 
 *  int_to_pixel
 *	 
 *	 
 *	
 *	
 * INPUTS: integer -- represents the color to convert where:
 * bits 31-24 are ignored
 * bits 23-16 are red
 * bits 15-8 are green
 * bits 7-0 are blue
 * OUTPUTS: none
 * RETURN VALUE: a pixel_t that represents the same color as the integer given
 * SIDE EFFECTS: none
 */


/*
 * bits 31-24 are ignored
 * bits 23-16 are red
 * bits 15-8 are green
 * bits 7-0 are blue
 */
static pixel_t int_to_pixel(int32_t integer){
	pixel_t pixel;
	pixel.red = (unsigned char) ((integer & 0x00FF0000) >> 16);
	pixel.green = (unsigned char) ((integer & 0x0000FF00) >> 8);
	pixel.blue = (unsigned char) (integer & 0x000000FF);
	return pixel;
}

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
 */

/*
 * Sets the file-scope variable color to be whatever the user wants it to be
 */
void set_color(int32_t new_color){
	color = new_color;
	return;
}

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
 */
int32_t draw_dot(int32_t x, int32_t y){
	if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
		picture_data[y*WIDTH + x] = int_to_pixel(color);
		return 1;
	}
	else{
		return 0;
	}
}


/* 
 *  make_png
 *	 
 *	 
 *	
 *	
 * INPUTS: str -- a string that tells the name of the file to write to
 *         also utilizes picture_data.
 * OUTPUTS: No direct outputs, creates a file given by str, after translating data from picture_data.
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */


static void make_png(char * str){
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int32_t status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int32_t pixel_size = 3;
    int32_t depth = 8;

    fp = fopen (str, "wb");
    if (! fp) {
        //goto fopen_failed;
				fprintf(stderr, "Failed to get file.");
				return;

    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
			fprintf(stderr, "PNG Failure. /n Failed to create PNG write struct.");
			fclose (fp);
	        //fopen_failed:
			return;

			//goto png_create_write_struct_failed;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        //goto png_create_info_struct_failed; // Ew GOTOs are supposed to be evil. Even though they were decent here.
				png_destroy_write_struct (&png_ptr, &info_ptr);
		 //png_create_write_struct_failed:
				fclose (fp);
		 //fopen_failed:
				return;
		}

    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        //goto png_failure;
				// If anyone does read this. Remember kids, GOTOs do not exist.
				png_destroy_write_struct (&png_ptr, &info_ptr);
		 //png_create_write_struct_failed:
				fclose (fp);
		 //fopen_failed:
				return;
    }

    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  WIDTH,
                  HEIGHT,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, HEIGHT * sizeof (png_byte *));
    for (y = 0; y < HEIGHT; y++) {
        png_byte *row =
            png_malloc (png_ptr, sizeof (unsigned char) * WIDTH * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < WIDTH; x++) {
            pixel_t pixel = picture_data[y*WIDTH + x];
            *row++ = pixel.red;
            *row++ = pixel.green;
            *row++ = pixel.blue;
        }
    }

    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status++;

    for (y = 0; y < HEIGHT; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);

 //png_failure:
 //png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 //png_create_write_struct_failed:
    fclose (fp);
 //fopen_failed:
    return;
}
