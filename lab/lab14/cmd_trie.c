#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_trie.h"


struct cmd_trie_t {
    cmd_func_t  func;     // NULL for non-existent command
    cmd_trie_t* next[26]; // next trie node
};


// Convert English (only) alphabetic characters to lower case.

static char 
English_to_lower (char c)
{
    return (c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c);
}


// Insert a command into a command trie.  The first argument should point
// to a variable holding the root of the tree, which should be initialized
// to NULL.  The command word s should contain only alphabetic characters 
// (case insensitive).  The function func is invoked if the command s is 
// passed to cmd_trie_execute.  Returns CT_SUCCESS on success, a negative
// number on failure.

int32_t 
cmd_trie_insert (cmd_trie_t** tptr, const char* s, cmd_func_t f)
{
    char c;	// next character in string

    // Check arguments.
    if (NULL == tptr || NULL == s || NULL == f) {
        return CT_BAD_ARGUMENTS;
    }

    // Read next character in string and convert to lower case.
    c = English_to_lower (*s);

    // Is the next character legal?  If not, fail.
    if ((c < 'a' || c > 'z') && '\0' != c) {
        return CT_ILLEGAL_COMMAND;
    }

    // Make a new trie node if necessary.  Initialize all fields to NULL.
    if (NULL == *tptr &&
        NULL == (*tptr = (cmd_trie_t*)calloc (sizeof (**tptr), 1))) {
	return CT_OUT_OF_MEMORY;
    }

    // Found the end of the command?  If so, add it to the trie.
    if ('\0' == c) {
	// Fail if the command already exists.
	if (NULL != (*tptr)->func) {
	    return CT_COMMAND_EXISTS;
	}
        (*tptr)->func = f;
	return CT_SUCCESS;
    }

    // Recurse using tail recursion.
    return cmd_trie_insert (&(*tptr)->next[c - 'a'], s + 1, f);
}


// Execute a command.  If a matching command is found in the command
// trie, its function is invoked, and the value returned from that function
// is returned from this function.  If no such command is found, this
// function returns NO_SUCH_COMMAND.

int32_t 
cmd_trie_execute (cmd_trie_t* t, const char* s)
{
    char c;	// next character in string

    // Check arguments.
    if (NULL == t || NULL == s) {
        return CT_BAD_ARGUMENTS;
    }

    // Skip any leading space characters.
    while (' ' == *s) { s++; }

    // Walk down trie.
    while (NULL != t) {

	// Read next character in string, advance string pointer,
	// and convert to lower case.
	c = English_to_lower (*s++);

	// Is the next character legal?  If not, the command is done, 
	// and we should try to execute.  If a command function is 
	// defined, invoke it on the rest of the string.
	if (c < 'a' || c > 'z') {	// note: includes '\0'
	    if (NULL != t->func) {
		return (*t->func) (s);
	    }
	    return CT_NO_SUCH_COMMAND;
	}

	// Move to next node based on c.
	t = t->next[c - 'a'];
    }

    // Failed to find command.
    return CT_NO_SUCH_COMMAND;
}


// Free all data associated with the command trie.

void 
cmd_trie_free (cmd_trie_t* t)
{
    int32_t i;

    for (i = 0; 26 > i; i++) {
        if (NULL != t->next[i]) {
	    cmd_trie_free (t->next[i]);
	}
    }
    free (t);
}


// Recursive function to list all commands.

static void
do_list_commands (cmd_trie_t* t, char* out, size_t pos, size_t len)
{
    int32_t i;

    // If function is defined for this node, print the command string.
    if (NULL != t->func) {
        out[pos] = '\0';
	puts (out);
    }

    // Space for longer commands in the buffer?
    if (len - 1 > pos) {

	// Print all nodes recursively (if they exist).
	for (i = 0; 26 > i; i++) {
	    if (NULL != t->next[i]) {
		out[pos] = 'a' + i;
		do_list_commands (t->next[i], out, pos + 1, len);
	    }
	}
    }
}


// List all commands, preceded by a prefix string.

void
cmd_trie_list_commands (cmd_trie_t* t, const char* prefix)
{
    char   output[200];	// buffer for command output
    size_t len;		// length of prefix string

    // Check arguments.
    if (NULL == t || NULL == prefix) { 
	return; 
    }

    // Fail silently for overly long prefix strings.
    if (199 < (len = strlen (prefix))) {
	return; 
    }

    // Copy the prefix to the output.
    strcpy (output, prefix);

    // Do the work.
    do_list_commands (t, output, len, 200);
}

