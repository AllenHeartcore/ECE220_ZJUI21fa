#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmd_trie.h"
#include "double_list.h"
#include "lab14.h"


// limit on input line size
#define CMD_BUF_SIZE 200


// list of people--initialized to an empty list
double_list_t people = {&people, &people};

// record of quit command
static int32_t quit_executed = 0;


// Convert English (only) alphabetic characters to upper case.

char 
English_to_upper (char* cptr)
{
    if (*cptr >= 'a' && *cptr <= 'z') {
        *cptr += 'A' - 'a';
    }
    return *cptr;
}


// Returns 1 if the given name appears on a person's team, or 0 if not.

static int32_t
is_on_team (const char* n, person_t* p)
{
    // Check the names for all teammates.
    for ( ; NULL != p; p = p->paired) {
	if (0 == strcasecmp (p->name, n)) {
	    return 1;
	}
    }

    // Not on this team.
    return 0;
}


// Given a name (in arg), find the person's team leader, if any.

static dl_execute_response_t
do_find (void* dl, void* arg)
{
    // Note that the team leader is returned, not the one named.
    return (is_on_team (arg, dl) ? DL_STOP_AND_RETURN : DL_CONTINUE);
}


// Given a name (in arg), extract the person's team, if any.

static dl_execute_response_t
do_extract (void* dl, void* arg)
{
    // Note that the team leader is removed from the list.
    // (Only the leader is in the list.)
    return (is_on_team (arg, dl) ? DL_REMOVE_AND_STOP : DL_CONTINUE);
}


// Execute an "add" command.  Returns a negative value for failures,
// 0 for bad arguments, and a positive value for success.

static int32_t
cmd_add (const char* s)
{
    char      newname[CMD_BUF_SIZE];	// new person's name
    char      newsex[3];		// new person's sex
    int32_t   newage;			// new person's age
    char      trash[2];			// for syntax checking
    person_t* p;			// new person to add

    // Check command syntax.
    if (3 != sscanf (s, "%s%2s%d%1s", newname, newsex, &newage, trash) ||
        '\0' == newname[0] || '\0' != newsex[1] || 
	('F' != English_to_upper (newsex) && 'M' != newsex[0]) ||
	1 > newage || 500 < newage) {
	puts ("syntax: add <name> <F|M> <age>");
        return 0;
    }

    // Check whether the name is already in use.
    if (NULL != dl_execute_on_all (&people, &do_find, newname)) {
        printf ("%s already exists.\n", newname);
	return 0;
    }

    // Allocate a new person and a copy of the name.
    if (NULL == (p = (person_t*)malloc (sizeof (*p))) ||
        NULL == (p->name = strdup (newname))) {
	if (NULL != p) {
	    free (p);
	}
	puts ("out of memory");
        return 1;	// no additional message needed
    }

    // Initialize the rest of the structure.
    p->age    = newage;
    p->sex    = newsex[0];
    p->paired = NULL;

    // Add the new person to the list of people.
    dl_insert (&people, &p->dl);

    // Print a feedback message.
    printf ("Added %s, %c, age %d.\n", newname, newsex[0], newage);
    return 1;
}


// Pretty-print individuals with names matching a prefix (in arg).  Note that
// anyone on a team is not counted as an individual.

static dl_execute_response_t
do_list_individuals_prefix (void* dl, void* arg)
{
    person_t* p = dl;	// person passed as dl 
    char*     s = arg;	// name prefix passed as arg

    if (NULL == p->paired && 0 == strncasecmp (p->name, s, strlen (s))) {
	printf ("  %s (%c%d)\n", p->name, p->sex, p->age);
    }
    return DL_CONTINUE;
}


// Pretty-print teams including any names matching a prefix (in arg).  

static dl_execute_response_t
do_list_teams_prefix (void* dl, void* arg)
{
    person_t* lead = dl;	// person passed as dl

    // Person must be on a team to be printed.
    if (NULL != lead->paired) {
	char*     s     = arg;		// name prefix passed as arg
	size_t    len   = strlen (s);	// length of prefix
	person_t* p;			// person iteration variable

	// Check all teammates' names.
	for (p = lead; NULL != p; p = p->paired) {
	    if (0 == strncasecmp (p->name, s, len)) {

		// Matching member found: print the team.
		printf ("  %s (%c%d)", lead->name, lead->sex, lead->age);
		for (p = lead->paired; NULL != p; p = p->paired) {
		    printf (", %s (%c%d)", p->name, p->sex, p->age);
		}
		puts ("");

		break;
	    }
	}
    }
    return DL_CONTINUE;
}


// Execute a "list" command.  Returns a negative value for failures,
// 0 for bad arguments, and a positive value for success.

static int32_t
cmd_list (const char* s)
{
    char prefix[CMD_BUF_SIZE];	// name prefix
    char trash[2];		// for syntax checking
    int  rval;			// return value from sscanf

    // Check command syntax--zero or one arguments allowed.
    if (1 < (rval = sscanf (s, "%s%1s", prefix, trash))) {
	puts ("syntax: list [<prefix>]");
        return 0;
    }
    
    // No argument--list everyone (empty string prefix).
    if (-1 == rval) {
        prefix[0] = '\0';
    }

    // Print the list.
    puts ("Individuals:");
    dl_execute_on_all (&people, &do_list_individuals_prefix, prefix);
    puts ("\nTeams:");
    dl_execute_on_all (&people, &do_list_teams_prefix, prefix);

    // Return success.
    return 1;
}


// Execute a "quit" command.  Returns a negative value for failures,
// 0 for bad arguments, and a positive value for success.

static int32_t
cmd_quit (const char* s)
{
    char trash[2];	// for syntax checking

    // Check command syntax.
    if (-1 == sscanf (s, "%1s", trash)) {

	// Mark file-scope variable to exit loop in main.
        quit_executed = 1;

	// Return success.
	return 1;
    }

    // Give feedback and return failure.
    puts ("syntax: quit");
    return 0;
}


// Execute a "pair" command--merges two teams.  Returns a negative value 
// for failures, 0 for bad arguments, and a positive value for success.

static int32_t
cmd_pair (const char* s)
{
    char leader_name[CMD_BUF_SIZE];	// leader's name
    char teammate_name[CMD_BUF_SIZE];	// teammate's name
    char trash[2];			// for syntax checking
    person_t* lead;			// leader of first team
    person_t* teammate;			// leader of second team
    person_t** list_end;		// points to NULL at end of first team

    // Check command syntax.
    if (2 != sscanf (s, "%s%s%1s", leader_name, teammate_name, trash)) {
	puts ("syntax: pair <name1> <name2>");
	return 0;
    }

    // Find the first team.
    if (NULL == (lead = dl_execute_on_all (&people, &do_find, leader_name))) {
        printf ("Who is %s?  Use full names for pair commands.\n", 
		leader_name);
	return 0;
    }

    // Find the second team.
    if (NULL == (teammate = dl_execute_on_all (&people, &do_extract,
					       teammate_name))) {
        printf ("Who is %s?  Use full names for pair commands.\n", 
		teammate_name);
	return 0;
    }

    // Merge teammate's team with leader's team.
    for (list_end = &lead->paired; NULL != (*list_end); 
    	 list_end = &(*list_end)->paired) { }
    *list_end = teammate;

    // Print a feedback message.
    printf ("Team of %s merged with team of %s.\n", 
    	    teammate_name, leader_name);
    return 1;
}


// Unpair the person with name matching n from the team starting with
// person p.  The person p must have a team, and the person with name n
// must be in that person's team.  Returns a pointer to the person with 
// name matching n.

static person_t*
do_unpair (person_t* p, const char* n)
{
    person_t* unpaired;

    if (0 == strcasecmp (p->name, n)) {
	// Named person is the leader.  Add next teammate to people and
	// set leader's team to NULL.
	dl_insert (&people, &p->paired->dl);
	p->paired = NULL;
    } else {
	for ( ; NULL != p->paired; p = p->paired) {
	    if (0 == strcasecmp (p->paired->name, n)) {
		// Found the person to be removed from the team (p->paired).
		// Remove them from the linked list of the team and insert
		// them into the people list with an empty team.
		unpaired = p->paired;
		p->paired = unpaired->paired;
		unpaired->paired = NULL;
		dl_insert (&people, &unpaired->dl);
		p = unpaired;
		break;
	    }
	}
    }
    // Print a feedback message.
    printf ("%s removed from team.\n", n);
    return p;
}


// Execute an "unpair" command.  Returns a negative value for failures, 
// 0 for bad arguments, and a positive value for success.

static int32_t
cmd_unpair (const char* s)
{
    char      n[CMD_BUF_SIZE];	// name to unpair
    char      trash[2];		// for syntax checking
    person_t* p;		// person to be unpaired

    // Check command syntax.
    if (1 != sscanf (s, "%s%1s", n, trash)) {
	puts ("syntax: unpair <name>");
	return 0;
    }

    // Find the person's team leader.
    if (NULL == (p = dl_execute_on_all (&people, &do_find, n))) {
        printf ("Who is %s?  Use full names for unpair commands.\n", n);
	return 0;
    }

    // Check that they're on a team.
    if (NULL == p->paired) {
        printf ("%s is not on a team!\n", n);
	return 0;
    }

    // Do the actual work of unpairing (also needed for deletion).
    (void)do_unpair (p, n);

    // Return success.
    return 1;
}


// Execute a "delete" command.  Returns a negative value for failures, 
// 0 for bad arguments, and a positive value for success.

static int32_t
cmd_delete (const char* s)
{
    char n[CMD_BUF_SIZE];	// name to delete
    char trash[2];		// for syntax checking
    person_t* p;		// person to delete

    // Check command syntax.
    if (1 != sscanf (s, "%s%1s", n, trash)) {
	puts ("syntax: delete <name>");
	return 0;
    }

    // Find the person's team leader.
    if (NULL == (p = dl_execute_on_all (&people, &do_find, n))) {
        printf ("Who is %s?  Use full names for delete commands.\n", n);
	return 0;
    }

    // If they're part of a team, remove them from the team and get
    // a pointer to the person to delete.
    if (NULL != p->paired) {
	p = do_unpair (p, n);
    }

    // Remove the person from the list of people and free their data.
    dl_remove (&p->dl);
    free (p->name);
    free (p);

    // Print a feedback message.
    printf ("%s deleted.\n", n);
    return 1;
}


// Add all people from a file.  Syntax is the same as for add commands.

static void
add_from_file (const char* fname) 
{
    FILE* f;			// the input stream
    char  buf[CMD_BUF_SIZE];	// a line from the input

    // Try to open the file.
    if (NULL == (f = fopen (fname, "r"))) {
        printf ("Could not open file '%s'.\n", fname);
	return;
    }

    // Parse each line as an "add" command.
    while (NULL != fgets (buf, CMD_BUF_SIZE, f)) {
	(void)cmd_add (buf);
    }

    // Close the file.
    (void)fclose (f);
}


// Delete all people.

static dl_execute_response_t
do_delete_all (void* dl, void* arg)
{
    person_t* p = dl;	// the person to be freed
    person_t* team;	// iteration over teammates
    person_t* next;	// need to read next link before freeing

    // Free all teammates' data.
    for (team = p->paired; NULL != team; team = next) {
        next = team->paired;
	free (team->name);
	free (team);
    }

    // Free p's name.
    free (p->name);

    // Cannot free p--need to have iteration code do that.
    return DL_FREE_AND_CONTINUE;
}


int
main ()
{
    cmd_trie_t* t = NULL;		// the trie of commands
    char        buf[CMD_BUF_SIZE];	// a command from the user
    int32_t	rval;			// return value for command execution

    // Add all people in the 'students' file.
    add_from_file ("students");

    // Add commands to the trie (with functions for each).
    cmd_trie_insert (&t, "add", &cmd_add);
    cmd_trie_insert (&t, "delete", &cmd_delete);
    cmd_trie_insert (&t, "list", &cmd_list);
    cmd_trie_insert (&t, "pair", &cmd_pair);
    cmd_trie_insert (&t, "show", &cmd_show);
    cmd_trie_insert (&t, "quit", &cmd_quit);
    cmd_trie_insert (&t, "unpair", &cmd_unpair);

    // Print a prompt and read commands until quit command is executed
    // or input ends.
    while (!quit_executed && 0 < printf (">> ") && 
           NULL != fgets (buf, CMD_BUF_SIZE, stdin)) {

	// Execute the given command.
	rval = cmd_trie_execute (t, buf);

	// Print a response for unsuccessful commands.
	if (0 > rval) {
	    puts ("Bad command.\nCommands include:");
	    cmd_trie_list_commands (t, "  ");
	} else if (0 == rval) {
	    puts ("Command failed.");
	}
    }

    // Free the command trie data.
    cmd_trie_free (t);

    // Free all person data.
    dl_execute_on_all (&people, &do_delete_all, NULL);

    // Return success.
    return 0;
}

