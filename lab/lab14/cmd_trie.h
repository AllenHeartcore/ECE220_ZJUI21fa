#if !defined(CMD_TRIE_H)
#define CMD_TRIE_H

#include <stdint.h>

#include "cmd_trie.h"

//
// A command function is called when the first non-white-space word in the
// string matches the command word.  The string passed to the command
// function starts with the character immediately after the command.
//
// So, for example, if the command is "  show foo", the command "show" is
// looked up in the trie, and the corresponding command function is 
// executed with the string " foo".
//
// The value returned by the command function is then returned by the
// cmd_trie_execute function.
// 
typedef int32_t (*cmd_func_t) (const char* s);

// A command trie node.
typedef struct cmd_trie_t cmd_trie_t;

// Error values.
enum { 
    CT_BAD_ARGUMENTS   = -5,	// bad arguments passed
    CT_ILLEGAL_COMMAND = -4,	// command contains non-alphabetic character
    CT_OUT_OF_MEMORY   = -3,	// out of memory (malloc failed)
    CT_COMMAND_EXISTS  = -2,	// command already defined
    CT_NO_SUCH_COMMAND = -1 ,	// command not defined
    CT_SUCCESS         = 0
};


// Insert a command into a command trie.  The first argument should point
// to a variable holding the root of the tree, which should be initialized
// to NULL.  The command word s should contain only alphabetic characters 
// (case insensitive).  The function func is invoked if the command s is 
// passed to cmd_trie_execute.  Returns CT_SUCCESS on success, or a 
// negative number on failure.
extern int32_t cmd_trie_insert (cmd_trie_t** tptr, const char* s, 
				cmd_func_t f);

// Execute a command.  If a matching command is found in the command
// trie, its function is invoked, and the value returned from that function
// is returned from this function.  If no such command is found, this
// function returns NO_SUCH_COMMAND.
extern int32_t cmd_trie_execute (cmd_trie_t* t, const char* s);

// Free all data associated with the command trie.
extern void cmd_trie_free (cmd_trie_t* t);

// List all commands, preceded by a prefix string.
extern void cmd_trie_list_commands (cmd_trie_t* t, const char* prefix);

#endif // CMD_TRIE_H
