/*									tab:8
 *
 * ece220_label.h - label facility header for ECE190 compiler (F08 MP5)
 *
 * "Copyright (c) 2008-2018 by Steven S. Lumetta."
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
 * Author:	    Steve Lumetta
 * Version:	    2
 * Creation Date:   Thu Oct 16 14:20:17 2008
 * Filename:	    ece220_label.c
 * History:
 *	SL	2	Tue Apr 03 23:13:03 2018
 *		Changed 190 to 220 for ECE220.
 *	SL	1	Thu Oct 16 14:20:17 2008
 *		First written.
 */

#if !defined(ECE220_LABEL_H)
#define ECE220_LABEL_H


/* 
 * OPAQUE STRUCTURE DEFINITION
 */
typedef struct ece220_label_t ece220_label_t;


/* 
 * INTERFACE FUNCTIONS 
 */

/* label_create -- allocate a new unique label */
extern ece220_label_t* label_create ();

/* label_value -- find the value (a string) for a given label */
extern char* label_value (ece220_label_t* label);


#endif /* ECE220_LABEL_H */
