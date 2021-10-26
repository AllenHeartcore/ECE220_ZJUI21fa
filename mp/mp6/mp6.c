/*									tab:8
 *
 * main.c - skeleton source file for ECE220 image processing MP6
 *
 * "Copyright (c) 2021 by Steven S. Lumetta
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
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
 * Author:	    Steven S. Lumetta
 * Version:	    1
 * Creation Date:   19 July 2021
 * Filename:	    mp6.h
 * History:
 *	SSL	1	19 July 2021
 *		First written.
 */
#include "mp6.h"

//
// convert_RGB_to_HSL -- convert red, green, and blue image data into
//   HSL channels.  See documentation for specific equations to be used.
//
//   INPUTS: height -- number of rows in image
//           width -- number of columns in image
//           red -- red pixels (0-255) in column-major order
//           green -- green pixels (0-255) in column-major order
//           blue -- blue pixels (0-255) in column-major order
//   OUTPUTS: H -- hue (0-49151) in column-major order
//            S -- saturation (0-32768) in column-major order
//            L -- luma (0 to 510) in column-major order
//   RETURN VALUE: none
//   SIDE EFFECTS: none
//
void
convert_RGB_to_HSL (int32_t height, int32_t width, const uint8_t* red, 
		    const uint8_t* green, const uint8_t* blue,
		    uint16_t* H, uint16_t* S, uint16_t* L)
{
}

//
// convert_HSL_to_RGB -- convert HSL channel data into red, green, and 
//   blue image data.  See documentation for specific equations to be used.
//
//   INPUTS: height -- number of rows in image
//           width -- number of columns in image
//           H -- hue (0-49151) in column-major order
//           S -- saturation (0-32768) in column-major order
//           L -- luma (0 to 510) in column-major order
//   OUTPUTS: red -- red pixels (0-255) in column-major order
//            green -- green pixels (0-255) in column-major order
//            blue -- blue pixels (0-255) in column-major order
//   RETURN VALUE: none
//   SIDE EFFECTS: none
//
void
convert_HSL_to_RGB (int32_t height, int32_t width, const uint16_t* H, 
		    const uint16_t* S, const uint16_t* L, 
		    uint8_t* red, uint8_t* green, uint8_t* blue)
{
}

//
// compute_sobel_kernels -- compute X and Y Sobel edge detection kernels
//   using convolution.  Edge pixels may be ignored, and kernel outputs
//   for these pixels left undefined.
//
//   INPUTS: height -- number of rows in image
//           width -- number of columns in image
//           L -- luma (0-510) in column-major order
//   OUTPUTS: Gx -- Sobel X kernel pixels in column-major order
//            Gy -- Sobel Y kernel pixels in column-major order
//   RETURN VALUE: none
//   SIDE EFFECTS: none
//
void
compute_sobel_kernels (int32_t height, int32_t width, const uint16_t* L, 
                       int32_t* Gx, int32_t* Gy)
{
}

//
// equalize_intensities -- perform histogram equalization to re-map image
//   intensities.
//
//   INPUTS: height -- number of rows in image
//           width -- number of columns in image
//           L -- luma (0-510) in column-major order
//   OUTPUTS: L -- modified luma (0-510) in column-major order
//   RETURN VALUE: none
//   SIDE EFFECTS: none
//
void
equalize_intensities (int32_t height, int32_t width, uint16_t* L)
{
}

