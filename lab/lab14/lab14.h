#if !defined(LAB14_H)
#define LAB14_H

#include <stdint.h>

#include "double_list.h"


// We should put all of the code into one file and hide this
// structure definition, but I want students to have their own
// smaller source file.

typedef struct person_t person_t;
struct person_t {
    double_list_t dl;		// for inclusion in list of people
    				// (only team leaders are in people list)
    char*         name;		// name, dynamically allocated
    char	  sex;		// 'F' or 'M'
    int32_t       age;		// age in years
    person_t*	  paired;	// singly-linked list of partners
};


// Even worse ... a global variable!  See comment above.
// This is the list of all people.

extern double_list_t people;


// this function in lab14main.c is available for your use
// Convert English (only) alphabetic characters to upper case.
extern char English_to_upper (char* cptr);


// You must write this function in lab14.c.  You may want to look
// at how the "list" function works in lab14main.c.

// Execute a "show" command.  Returns a negative value for failures,
// 0 for bad arguments, and a positive value for success.
extern int32_t cmd_show (const char* s);

#endif // LAB14_H
