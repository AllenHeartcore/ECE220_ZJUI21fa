#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lab10.h"

#define MAX_FILES  10	// number of files allowed

#define MAX_WORD_LEN 500 // maximum length of a "word"


// code for process_file was adapted from word_split.c

static void
process_file (FILE* in_file, int32_t hashes[], char* words[], 
	      void (*word_func) (const char* one_word, int32_t hashes[], 
	      char* words[]))
{
    char    buf[MAX_WORD_LEN + 1];	/* holds current word     */
    char*   write;			/* end of current word    */
    int32_t word_len;			/* length of current word */
    int32_t a_char;			/* last character read    */

    /* Initialize the word writing variable to point to the start of
       the word buffer. */
    write = buf;
    word_len = 0;

    /* Read characters until we find the end of the input. */
    while (EOF != (a_char = getc (in_file))) {

	/* If necessary, change input character to lower case. */
	if ('A' <= a_char && 'Z' >= a_char) 
	    a_char = a_char - 'A' + 'a';

	/* Can character be part of a word? */
	if (('a' <= a_char && 'z' >= a_char) || 
	    (0 < word_len && ('-' == a_char || '\'' == a_char))) {

	    /* Write the character into our word buffer and increment
	       the pointer and counter. */
	    *write++ = a_char;
	    word_len++;

	    /* Do we still have room in the buffer?  If so, read
	       another character (skip to next loop iteration). */
	    if (MAX_WORD_LEN > word_len)
		continue;
	} else {
	    /* Invalid character read.  Is there a word that needs
	       to be written out?  If not, skip to next character. */
	    if (0 == word_len)
		continue;
	}

	/* Write out the current word, then reset the buffer pointer
	   and character count. */
	*write = 0;
	word_func (buf, hashes, words);
	write = buf;
	word_len = 0;
    }

    /* Any last words? */
    if (0 < word_len) {
	*write = 0;
	word_func (buf, hashes, words);
    }
}


static void
found_a_word (const char* one_word, int32_t hashes[], char* words[])
{
    int32_t i;

    // move words forward in queue
    if (NULL != words[0]) {
        free (words[0]);
    }
    for (i = 1; NUM_WORDS > i; i++) {
        words[i - 1] = words[i];
    }
    words[NUM_WORDS - 1] = strdup (one_word);
    // skip any partially empty tuples or tuples for which strdup fails
    for (i = 0; NUM_WORDS > i; i++) {
        if (NULL == words[i]) {
	    return;
	}
    }
    calculate_hashes (hashes, words);
}


static int32_t
parse_file (const char* fname, int32_t hashes[], char* words[])
{
    FILE*   f;
    int32_t i;

    if (NULL == (f = fopen (fname, "r"))) {
        return -1;
    }
    for (i = 0; NUM_HASHES > i; i++) {
        hashes[i] = INT32_MAX;
    }
    for (i = 0; NUM_WORDS > i; i++) {
        words[i] = NULL;
    }
    process_file (f, hashes, words, found_a_word);
    for (i = 0; NUM_WORDS > i; i++) {
	if (NULL != words[i]) {
	    free (words[i]);
	}
    }
    return 0;
}


int
main (int argc, const char* const argv[])
{
    int32_t     hashes[MAX_FILES][NUM_HASHES];
    char*       words[NUM_WORDS];
    const char* names[NUM_HASHES];
    int32_t     n_files;
    int32_t     i;
    int32_t     f1;
    int32_t     f2;
    int32_t     count;

    if (3 > argc || MAX_FILES + 1 < argc) {
        fprintf (stderr, "usage: %s <file1> <file2> ...\n", argv[0]);
        fprintf (stderr, "       (at most %d files)\n", MAX_FILES);
	return 2;
    }

    for (n_files = 0, i = 1; argc > i; i++) {
        if (0 == parse_file (argv[i], hashes[n_files], words)) {
	    names[n_files++] = argv[i];
	} else {
	    fprintf (stderr, "could not parse file \"%s\"\n", argv[i]);
	}
    }

    // do the comparison now
    for (f1 = 0; n_files > f1; f1++) {
	for (f2 = f1 + 1; n_files > f2; f2++) {
	    for (i = 0, count = 0; NUM_HASHES > i; i++) {
	        count += (hashes[f1][i] == hashes[f2][i]);
	    }
	    if (0 < count) {
	        printf ("%3.2f%% similar: %s and %s\n",
			100 * count / (double)NUM_HASHES,
			names[f1], names[f2]);
	    }
	}
    }

    return 0;
}

