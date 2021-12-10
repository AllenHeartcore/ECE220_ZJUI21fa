#include <stdint.h>
#include <stdio.h>

#include "lab14.h"


// Pretty-print individuals with matching sex (in *arg).  Note that
// anyone on a team is not counted as an individual.

static dl_execute_response_t
do_show_individuals (void* dl, void* arg)
{
    // LAB14: Given a person_t (passed as dl) and a sex 
    //        (passed using arg--you decide exactly how),
    //        check whether the person is NOT on a team and
    //        whether the person's sex matches the requested 
    //        sex.  If both conditions are met, print the 
    //        person's name, sex, and age using the format 
    //        shown.

    // format for printf: "  %s (%c%d)\n" (name, sex, age)

    // LAB14: return the appropriate DL_* value
}


// Pretty-print teams including anyone with matching sex (in *arg).  

static dl_execute_response_t
do_show_teams (void* dl, void* arg)
{
    // LAB14: You may want to read do_list_teams_prefix in lab14main.c
    //        before writing this function.  The format is identical;
    //        you just need different conditions for printing.

    // LAB14: Given a person_t (passed as dl) and a sex 
    //        (passed using arg--you decide exactly how),
    //        check whether the person IS on a team and
    //        whether any member of the team's sex matches
    //        the requested sex.  If both conditions are 
    //        met, print each member of the team's name, 
    //        sex, and age using the format shown, preceded
    //        by two spaces and separated by a comma and a
    //        space.

    // format for printf: "  %s (%c%d)\n" (name, sex, age)

    // LAB 14: return the appropriate DL_* value
}


// Execute a "show" command.  Returns a negative value for failures,
// 0 for bad arguments, and a positive value for success.

int32_t
cmd_show (const char* s)
{
    // Check command syntax.  See cmd_add for a similar example
    // (the "add" command also requires a sex).  If the syntax
    // is wrong, print the error message shown.
    if (1) { // LAB14: fix this condition to check syntax for "show" command
	puts ("syntax: show <F|M>");
        return 0;
    }
    
    // Print a list of individuals (those not on teams) of the requested sex.
    puts ("Individuals:");

    // LAB14: Execute do_show_individuals on all person_t's in the people list.
    //        Note that the function needs to know the requested sex.

    // Now print a list of teams that include at least one member of
    // the requested sex.
    puts ("\nTeams:");

    // LAB14: Execute do_show_teams on all person_t's in the people list.
    //        Note that the function needs to know the requested sex.

    // Return success.
    return 1;
}


