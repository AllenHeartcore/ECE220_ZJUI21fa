/*									tab:8
 *
 * main.c - source file for ECE220 picture drawing program wrapper code
 *
 * "Copyright (c) 2018-2021 by Charles H. Zega, and Saransh Sinha."
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
 * Author:	    Charles Zega, Saransh Sinha, Steven S. Lumetta
 * Version:	    3
 * Creation Date:   12 February 2018
 * Filename:	    mp6.h
 * History:
 *	CZ	1	12 February 2018
 *		First written.
 *	SSL	2	13 February 2018
 *		Split into separate C files for compilation.  Made local
 *              functions and structures file scope.
 *      SSL     3       19 July 2021
 *              Adapted for new MP6.  Added PNG reading.
 */

#include <math.h>
#include <png.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mp6.h"

// an image, for the purposes of this MP
typedef struct image_t image_t;
struct image_t {
    int32_t height;
    int32_t width;
    uint8_t* red;
    uint8_t* green;
    uint8_t* blue;
};

static image_t* read_png (const char* fname);
static int32_t write_png (const char* fname, image_t* image);
static void free_image (image_t* image);
static double get_sobel_bound 
	(image_t* image, const int32_t* Gx, const int32_t* Gy);
static void show_sobel_image 
	(image_t* image, const int32_t* Gx, const int32_t* Gy);
static void sharpen_edges 
	(image_t* image, const int32_t* Gx, const int32_t* Gy, uint16_t* L);
static int32_t write_HSL (const char* fname, image_t* image, 
	const uint16_t* H, const uint16_t* S, uint16_t* L);

static void
usage (const char* ename)
{
    fprintf (stderr, "syntax: %s <PNG input file> <output file> "
	     "<transform #>\n", ename);
    fprintf (stderr, "        transform 0 writes human-readable HSL\n");
    fprintf (stderr, "        transform 1 is RGB->HSL->RGB\n");
    fprintf (stderr, "        transform 2 is edge detection\n");
    fprintf (stderr, "        transform 3 is edge sharpening\n");
    fprintf (stderr, "        transform 4 is all + equalization\n");
}

int 
main (int argc, const char* const argv[])
{
    image_t* image;
    int32_t npix;
    uint16_t* H;
    uint16_t* S = NULL;
    uint16_t* L = NULL;
    int32_t* Gx = NULL;
    int32_t* Gy = NULL;
    int32_t trans;

    if (4 != argc) {
	usage (argv[0]);
	return 2;
    }
    trans = atoi (argv[3]);
    if (0 > trans || 4 < trans) {
	usage (argv[0]);
	return 2;
    }

    if (NULL == (image = read_png (argv[1]))) {
	return 3;
    }

    // Compute number of pixels, then allocate space for HSL data.
    npix = image->height * image->width;
    if (NULL == (H = (uint16_t*)malloc (sizeof (*H) * npix)) ||
	NULL == (S = (uint16_t*)malloc (sizeof (*S) * npix)) ||
	NULL == (L = (uint16_t*)malloc (sizeof (*L) * npix)) ||
	NULL == (Gx = (int32_t*)malloc (sizeof (*Gx) * npix)) ||
	NULL == (Gy = (int32_t*)malloc (sizeof (*Gy) * npix))) {
	perror ("allocate space for HSL, Gx, Gy");
	free (H);
	free (S);
	free (L);
	free (Gx);
	free (Gy);
	free_image (image);
	return 3;
    }

    convert_RGB_to_HSL (image->height, image->width, image->red, 
    			image->green, image->blue, H, S, L);
    if (0 == trans) {
        // HSL output as text
	int32_t rval = write_HSL (argv[2], image, H, S, L);
	free_image (image);
	return (1 == rval ? 0 : 3);
    }
    if (1 < trans) {
	compute_sobel_kernels (image->height, image->width, L, Gx, Gy);
	if (2 == trans) {
	    show_sobel_image (image, Gx, Gy);
	} else {
	    sharpen_edges (image, Gx, Gy, L);
	    if (4 == trans) {
	        equalize_intensities (image->height, image->width, L);
	    }
	}
    }
    if (2 != trans) {
	// clear RGB data to avoid confusion
	memset (image->red, 0, 
		sizeof (image->red[0]) * image->height * image->width);
	memset (image->green, 0, 
		sizeof (image->green[0]) * image->height * image->width);
	memset (image->blue, 0, 
		sizeof (image->blue[0]) * image->height * image->width);
	convert_HSL_to_RGB (image->height, image->width, H, S, L,
			    image->red, image->green, image->blue);
    }

    if (0 != write_png (argv[2], image)) {
	free_image (image);
        return 3;
    }

    free_image (image);
    return 0;
}

/* 
 * free_image -- free data for an image
 *   INPUTS: image -- a pointer to an image
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: frees the image and channel data
 */

static void 
free_image (image_t* image)
{
    free (image->red);
    free (image->green);
    free (image->blue);
    free (image);
}

/* 
 * read_png -- allocates and fills in an image from a PNG file
 *   INPUTS: fname -- the name of the input PNG file
 *   OUTPUTS: none
 *   RETURN VALUE: an image, or NULL on failure
 *   SIDE EFFECTS: none
 */
static image_t* 
read_png (const char* fname) 
{
    FILE*       f;		// file to read
    png_structp png;		// PNG data pointer
    png_infop   info;		// PNG info pointer (header)
    png_byte    color_type;	// PNG color type (RGB or RGBA only)
    int32_t     height, width;	// image height and width
    int32_t     channel_bytes;	// bytes per channel
    png_byte**  rows;		// pointer to PNG row data
    int32_t     i;      	// index over rows
    int32_t     j;      	// index over columns
    int32_t     idx;		// index over RGB data in image
    int32_t     p;		// index over RGB data in PNG
    image_t*    res;		// image structure to return

    // Open the input file.
    if (NULL == (f = fopen (fname, "rb"))) {
	perror ("open input PNG file");
	return NULL;
    }

    // Create a PNG structure for reading the file.
    if (NULL == (png = png_create_read_struct 
    		 (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
	fputs ("libpng failure to create read structure\n", stderr);
	fclose (f);
	return NULL;
    }

    // Create an info structure to hold header information.
    if (NULL == (info = png_create_info_struct (png))) {
	fputs ("libpng failure to create info structure\n", stderr);
	// Passing a pointer to NULL for the second argument causes
	// a crash in libpng.  We have no info structure to destroy.
	png_destroy_read_struct (&png, NULL, NULL);
	fclose (f);
	return NULL;
    }

    // Set up error handling.
    if (setjmp (png_jmpbuf (png))) {
	fputs ("libpng failure\n", stderr);
	png_destroy_read_struct (&png, &info, NULL);
	fclose (f);
	return NULL;
    }

    // Initialize I/O, then read the PNG header.
    png_init_io (png, f);
    png_read_info (png, info);

    // Check for acceptable image formats.
    color_type = png_get_color_type (png, info);
    if ((PNG_COLOR_TYPE_RGB != color_type &&
	 PNG_COLOR_TYPE_RGB_ALPHA != color_type) ||
	 8 != png_get_bit_depth (png, info)) {
	fputs ("input PNG is not 8-bit RGB(A)\n", stderr);
	png_destroy_read_struct (&png, &info, NULL);
	fclose (f);
	return NULL;
    }

    // Discard any alpha channel.
    if (PNG_COLOR_TYPE_RGB_ALPHA == color_type) {
        png_set_strip_alpha (png);
        png_read_update_info (png, info);
    }

    // Read height and width.
    height = png_get_image_height (png, info);
    width  = png_get_image_width (png, info);
    channel_bytes = height * width;

    // Allocate space for the image data.  Note that png_malloc invokes
    // error handling if out of memory (see setjmp above).
    rows = (png_byte**)png_malloc (png, sizeof (*rows) * height);
    for (i = 0; height > i; i++) {
        rows[i] = (png_byte*)png_malloc (png, 3 * sizeof (**rows) * width);
    }

    // Read the image!
    png_read_image (png, rows);

    // Allocate space for an image.
    if (NULL == (res = (image_t*)malloc (sizeof (*res)))) {
        perror ("allocate image");
    } else {
	// Fill in the image.
	res->height = height;
	res->width = width;
	res->red = res->green = res->blue = NULL;
	if (NULL == (res->red = (uint8_t*)malloc 
		    (channel_bytes * sizeof (res->red[0]))) ||
	    NULL == (res->green = (uint8_t*)malloc 
		    (channel_bytes * sizeof (res->green[0]))) ||
	    NULL == (res->blue = (uint8_t*)malloc 
		    (channel_bytes * sizeof (res->blue[0])))) {
	    // Out of memory.
	    perror ("allocate image channels");
	    free (res->red);
	    free (res->green);
	    free (res->blue);
	    free (res);
	    res = NULL;
	} else {
	    // Copy PNG data to image.
	    for (i = idx = 0; height > i; i++) {
		for (j = p = 0; width > j; j++, idx++, p += 3) {
		    res->red[idx]   = rows[i][p + 0];
		    res->green[idx] = rows[i][p + 1];
		    res->blue[idx]  = rows[i][p + 2];
		}
	    }
	}
    }

    // Free the PNG memory and structures.
    for (i = 0; height > i; i++) {
        png_free (png, rows[i]);
    }
    png_free (png, rows);
    png_destroy_read_struct (&png, &info, NULL);

    fclose (f);
    return res;
}


/* 
 * write_png -- write a PNG file from an image
 *   INPUTS: fname -- a string that tells the name of the file to write
 *           image -- the image
 *   OUTPUTS: none
 *   RETURN VALUE: 1 on success, or 0 on failure
 *   SIDE EFFECTS: creates the output file
 */
static int32_t 
write_png (const char* fname, image_t* image) 
{
    FILE* fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte** row_pointers = NULL;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int32_t pixel_size = 3;
    int32_t depth = 8;

    if (NULL == (fp = fopen (fname, "wb"))) {
	fprintf (stderr, "Failed to open file.\n");
	return 0;
    }

    if (NULL == (png_ptr = png_create_write_struct 
    		 (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
	fprintf (stderr, "PNG Failure.\nFailed to create PNG write struct.\n");
	fclose (fp);
	return 0;
    }

    if (NULL == (info_ptr = png_create_info_struct (png_ptr))) {
	fprintf (stderr, "PNG Failure.\nFailed to create PNG info struct.\n");
	png_destroy_write_struct (&png_ptr, &info_ptr);
	fclose (fp);
	return 0;
    }

    /* Set up error handling. */
    if (setjmp (png_jmpbuf (png_ptr))) {
	fprintf (stderr, "PNG Failure.\n");
	png_destroy_write_struct (&png_ptr, &info_ptr);
	fclose (fp);
	return 0;
    }

    /* Set image attributes. */
    png_set_IHDR (png_ptr, info_ptr, image->width, image->height, depth, 
    		  PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, 
		  PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = png_malloc (png_ptr, image->height * sizeof (png_byte *));
    for (y = 0; y < image->height; y++) {
        png_byte *row = png_malloc 
		(png_ptr, sizeof (png_byte) * image->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < image->width; x++) {
	    *row++ = image->red[y * image->width + x];
	    *row++ = image->green[y * image->width + x];
	    *row++ = image->blue[y * image->width + x];
        }
    }

    /* Write the image data to "fp". */
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for (y = 0; y < image->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);

    png_destroy_write_struct (&png_ptr, &info_ptr);
    fclose (fp);
    return 1;
}

/* 
 * get_sobel_bound -- calculate the maximum Sobel gradient length; ignores
 *                    values corresponding to image edges
 *   INPUTS: image -- the image--height and width are used
 *           Gx -- the X kernel in row-major order
 *           Gy -- the Y kernel in row-major order
 *   OUTPUTS: none
 *   RETURN VALUE: the maximum length found
 *   SIDE EFFECTS: none
 */
static double
get_sobel_bound (image_t* image, const int32_t* Gx, const int32_t* Gy)
{
    int32_t idx;
    int32_t x;
    int32_t y;
    double amt;
    double max = -1;

    // find a bound on Sobel distance
    for (idx = image->width, y = 1; image->height - 1 > y; y++) {
        idx++;
	for (x = 1; image->width - 1 > x; x++, idx++) {
	    amt = sqrt (Gx[idx] * (double) Gx[idx] + 
	    		Gy[idx] * (double) Gy[idx]);
	    if (max < amt) {
	        max = amt;
	    }
	}
        idx++;
    }
    return max;
}

/* 
 * show_sobel_image -- make an image to show Sobel kernel results; image
 *                     size defined by the image input; edges are ignored
 *                     and set to black in output
 *   INPUTS: image -- the image--height and width are used
 *           Gx -- the X kernel in row-major order
 *           Gy -- the Y kernel in row-major order
 *   OUTPUTS: image -- the image red, green, and blue channels
 *   RETURN VALUE: 1 on success, or 0 on failure
 *   SIDE EFFECTS: none
 */
static void 
show_sobel_image (image_t* image, const int32_t* Gx, const int32_t* Gy)
{
    double max;
    int32_t idx;
    int32_t x;
    int32_t y;
    double amt;
    uint8_t pval;

    // find a bound on Sobel distance
    max = get_sobel_bound (image, Gx, Gy);

    // first row is black
    for (idx = x = 0; image->width > x; x++, idx++) {
        image->red[idx] = image->green[idx] = image->blue[idx] = 0;
    }
    for (y = 1; image->height - 1 > y; y++) {
	// first column is black
        image->red[idx] = image->green[idx] = image->blue[idx] = 0;
	idx++;

	for (x = 1; image->width - 1 > x; x++, idx++) {
	    amt = sqrt (Gx[idx] * (double) Gx[idx] + 
	    		Gy[idx] * (double) Gy[idx]) / max;
	    pval = 255 * amt;
	    image->red[idx] = image->green[idx] = image->blue[idx] = pval;
	}

	// last column is black
        image->red[idx] = image->green[idx] = image->blue[idx] = 0;
	idx++;
    }
}

static void 
sharpen_edges (image_t* image, const int32_t* Gx, const int32_t* Gy, 
	       uint16_t* L)
{
    int32_t w = image->width;
    double max;
    int32_t idx;
    int32_t x;
    int32_t y;
    int32_t ox, oy;
    static int32_t sx[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
    static int32_t sy[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
    double dx, dy;
    int32_t adjL;

    // find a bound on Sobel distance; only sharpen a little (the 10x)
    max = 20 * get_sobel_bound (image, Gx, Gy) / 510;

    for (idx = w, y = 1; image->height - 1 > y; y++) {
        idx++;
	for (x = 1; w - 1 > x; x++, idx++) {
	    dx = Gx[idx] / max;
	    dy = Gy[idx] / max;
	    for (oy = -1; 1 >= oy; oy++) {
		for (ox = -1; 1 >= ox; ox++) {
		    adjL = sx[oy + 1][ox + 1] * dx + sy[oy + 1][ox + 1] * dy;
		    adjL += L[idx + oy * w + ox];
		    L[idx + oy * w + ox] = 
			    (0 > adjL ? 0 : (510 < adjL ? 510: adjL));
		}
	    }
	}
        idx++;
    }
}

static int32_t 
write_HSL (const char* fname, image_t* image, 
	   const uint16_t* H, const uint16_t* S, uint16_t* L)
{
     FILE* f;
     int32_t x, y, p;

     if (NULL == (f = fopen (fname, "w"))) {
         perror ("fopen output");
	 return 0;
     }
     for (p = y = 0; image->height > y; y++) {
	 for (x = 0; image->width > x; x++, p++) {
	     fprintf (f, "H(%d,%d) = %d\n", x, y, H[p]);
	     fprintf (f, "S(%d,%d) = %d\n", x, y, S[p]);
	     fprintf (f, "L(%d,%d) = %d\n", x, y, L[p]);
	 }
     }
     (void)fclose (f);
     return 1;
}

