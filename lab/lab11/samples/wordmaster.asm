; ------------------------------------------------------------------------
;
; wordmaster
;
; A game similar in content to Hangman, but without all the nasty side
; effects of losing.  Instead, you play for points as follows...
;      bad letter    ->  -1 point
;      good letter   ->   0 points
;      finish a word ->   5 points
;
; You start with 20 points.  If you make it to 50, you win.  If you drop
; to zero...well...you can always try again.
;

	.ORIG x3000


	LEA R0,WELCOME_STR	; say hello
	PUTS

	LD R0,FUNC1		; seed the random number generator
	JSRR R0
	LD R0,FUNC2		; build the dictionary of words
	JSRR R0
	JSR PLAY		; play the game until you win or lose

	; R0 = 0 is a lose, 1 is a quit, 2 is a win

	LEA R1,TERM_STR_MAP	; print the right message and stop
	ADD R0,R0,R1
	LDR R0,R0,#0
	PUTS
	HALT

FUNC1	.FILL SRANDOM		; functions to execute...
FUNC2	.FILL BUILD_DICT

TERM_STR_MAP			; a table of possible termination strings
	.FILL LOSE_STR
	.FILL QUIT_STR
	.FILL WIN_STR


WELCOME_STR .STRINGZ "
  ********************************************************************  
 **                                                                  **
**                Welcome to wordmaster!  Good luck!                  **
 **                                                                  ** 
  ********************************************************************  

"

LOSE_STR .STRINGZ "
Oops!  Looks like you're out of points.  Better luck next time.
The Union bookstore sells dictionaries, you know.
"

QUIT_STR .STRINGZ "
Quitting already?  You can't be a word master unless you master the words,
you realize?
"

WIN_STR	.STRINGZ "
************************************************************************
****                                                                ****
**           Congratulations!  You are a master of words!             **
****                                                                ****
************************************************************************

"

; ------------------------------------------------------------------------
; the main game routines are in this section

; R5 points to the current word

PLAY	ST R7,PLAY_R7		; save R7 to allow further calls

PLAY_NEW_WORD
	JSR PICK_A_WORD		; pick a new word
PLAY_SAME_WORD
	JSR PRINT_STATUS	; print the status/word/letters tried
	JSR GUESS_A_LETTER	; handle one guess; returns R0=1 if 
				;    word is completed, 0 if not completed,
				;    and -1 for quitting the game

	; check score conditions
	LD R1,POINTS		; > 0 ?
	BRp NOT_LOST_YET

	AND R0,R0,#0		; nope, out of points...tough luck!
	BRnzp PLAY_DONE

NOT_LOST_YET
	LD R2,NEG_WIN		; enough points to win?
	ADD R2,R2,R1
	BRn NOT_WON_YET

	AND R0,R0,#0		; yes!  The player has won.
	ADD R0,R0,#2
	BRnzp PLAY_DONE

NOT_WON_YET
	ADD R0,R0,#0		; guess again or generate new word?
	BRz PLAY_SAME_WORD
	BRp PLAY_NEW_WORD

	AND R0,R0,#0		; player quit
	ADD R0,R0,#1

PLAY_DONE
	LD R7,PLAY_R7		; restore R7
	RET

PLAY_R7	.BLKW 1			; storage for R7
NEG_WIN	.FILL #-50		; score required to win (only prints
				;    2 digits, so don't raise > 100)

;
; -----------------------------------------------------------------------
; You have xx points.  The word is _ _ _ h a _ a _ _ .
;
; So far, you have tried: a b d f g h x y .
; -----------------------------------------------------------------------
;
 

PRINT_STATUS
	ST R7,PSTAT_R7		; save R7 to allow traps

	LEA R0,STAT_S1		; print first part of status string
	PUTS
	LD R0,ASCII_ZERO
	LD R1,POINTS

PSTAT_TENS			; calculate tens digit of points
	ADD R0,R0,#1
	ADD R1,R1,#-10
	BRzp PSTAT_TENS
	ADD R1,R1,#10
	ADD R0,R0,#-1
	OUT			; and print it

	LD R0,ASCII_ZERO	; then print ones digit of points
	ADD R0,R0,R1
	OUT

	LEA R0,STAT_S2		; print second part of status string
	PUTS
	LD R0,ASCII_ZERO
	LD R1,POINTS

	; print word with blanks for letters not yet guessed
	ADD R1,R5,#0		; R1 points to letters in word
	LEA R2,CORRECT		; R2 points to array of boolean values
				;    0 if not guessed, 1 if guessed

PSTAT_PRINT_WORD
	LDR R0,R1,#0		; end of string yet?
	BRz PSTAT_WORD_DONE

	LDR R3,R2,#0		; this letter guessed?
	BRnp PSTAT_PRINT_LET
	LD R0,A_BLANK		; if not, print a blank
PSTAT_PRINT_LET
	OUT

	LD R0,A_SPACE		; after letter/blank, print a space
	OUT

	ADD R1,R1,#1		; move on to next letter
	ADD R2,R2,#1
	BRnzp PSTAT_PRINT_WORD

PSTAT_WORD_DONE
	LEA R0,STAT_S3		; print third part of status string
	PUTS
	LD R0,ASCII_ZERO
	LD R1,POINTS

	; print the letters already tried for this word
	LD R1,ASCII_LC_A	; R1 holds ASCII lower case 'a'
	LEA R2,TRIED		; R2 points to array of boolean values
				;    0 if letter not tried, 1 if tried
	LD R3,NUM_LETTERS	; R3 counts down from 26

PSTAT_PRINT_TRIES
	LDR R4,R2,#0		; did the player try this letter?
	BRz PSTAT_NOT_TRIED

	ADD R0,R1,#0		; if so, print the letter and a space
	OUT
	LD R0,A_SPACE
	OUT

PSTAT_NOT_TRIED
	ADD R1,R1,#1		; loop back over all letters
	ADD R2,R2,#1
	ADD R3,R3,#-1
	BRp PSTAT_PRINT_TRIES


	LEA R0,STAT_S4		; print fourth part of status string
	PUTS
	LD R0,ASCII_ZERO
	LD R1,POINTS

	LD R7,PSTAT_R7		; restore R7
	RET


ASCII_ZERO	.FILL x30	; '0'
A_BLANK		.FILL x5F	; '_' for blanks
A_SPACE		.FILL x20	; ' '
ASCII_LC_A	.FILL x61	; 'a'
NUM_LETTERS	.FILL #26
STAT_S1	.STRINGZ "-----------------------------------------------------------------------\nYou have "
STAT_S2	.STRINGZ " points.  The word is: "
STAT_S3	.STRINGZ "\n\nSo far, you have tried: "
STAT_S4	.STRINGZ "\n"
;STAT_S1	.STRINGZ "You have "
;STAT_S2	.STRINGZ " points.  The word is "
;STAT_S3	.STRINGZ ".\n\nSo far, you have tried: "
;STAT_S4	.STRINGZ ".\n-----------------------------------------------------------------------\n"
PSTAT_R7	.BLKW 1


PICK_A_WORD
	ST R7,PAW_R7		; save R7

	AND R0,R0,#0

	LEA R5,TRIED		; zero out array of tried letters
	LD R1,NUM_LETTERS
CLEAR_TRIED
	STR R0,R5,#0
	ADD R5,R5,#1
	ADD R1,R1,#-1
	BRp CLEAR_TRIED
	
	LEA R5,CORRECT		; zero out array of guessed letters
	LD R1,MAXLEN
CLEAR_CORRECT
	STR R0,R5,#0
	ADD R5,R5,#1
	ADD R1,R1,#-1
	BRp CLEAR_CORRECT

	; pick a word at random
	JSR RANDOM		; get a random number in R0 (0...2^15-1)
	LDI R1,DICT_PTR		; read the number of words in R1
	JSR MOD			; use MOD to find a word 
	LD R5,DICT_PTR		; point R5 to the word
	ADD R5,R5,R0
	LDR R5,R5,#1		; words start at DICT_PTR + 1

	ADD R0,R5,#0		; count the letters in the word
	AND R1,R1,#0
PAW_FIND_LEN
	LDR R2,R0,#0
	BRz PAW_FOUND_LEN
	ADD R0,R0,#1
	ADD R1,R1,#1
	BRnzp PAW_FIND_LEN
PAW_FOUND_LEN
	ST R1,REMAIN		; all letters must be guessed

	LD R7,PAW_R7		; restore R7

	RET

PAW_R7	.BLKW 1


POINTS	.FILL #20	 	; start with 20 points
TRIED	.BLKW #26		; letter A...Z tried for this word?
REMAIN  .BLKW #1			; missing letters remaining
CORRECT	.BLKW #40		; letters in word have been guessed?
				;    (length is negative of NEG_MAXLEN 
				;    value later in code)
MAXLEN	.FILL #40		; also here...


GUESS_A_LETTER
	ST R7,GUESS_R7		; save R7 to allow traps

GUESS_LOOP
	GETC			; get a key
	
	LD R1,ASCII_NEG_ESC	; does player want to quit?
	ADD R1,R1,R0
	BRnp GUESS_NOT_ESC

	AND R0,R0,#0		; yes, so return R0=-1
	ADD R0,R0,#-1
	BRnzp GUESS_DONE

GUESS_NOT_ESC
	LD R1,ASCII_NEG_LC_A	; key in range 'a'...'z'?
	ADD R1,R0,R1		; < 'a' (including upper case!)
	BRn GUESS_LOOP
	LD R2,NEG_NUM_LETTERS	; > 'z'
	ADD R2,R2,R1
	BRzp GUESS_LOOP
	
	; note that R0 is still the ASCII character pressed, while
	; R1 is the corresponding index (0...25).

	LEA R2,TRIED		; was letter tried already?
	ADD R2,R2,R1
	LDR R3,R2,#0
	BRnp GUESS_LOOP

	ADD R3,R3,#1		; mark the letter as tried now
	STR R3,R2,#0

	; mark and count occurences in word
	NOT R0,R0		; negate character in R0 for comparisons
	ADD R0,R0,#1
	ADD R1,R5,#0		; point R1 to word
	LEA R2,CORRECT		; point R2 to array of correctly guessed
				;    letters
	AND R3,R3,#0		; count of guessed letter in word

GUESS_CHECK_WORD
	LDR R4,R1,#0		; read a character from the word
	BRz GUESS_END_WORD
	ADD R4,R4,R0		; is it a match?
	BRnp GUESS_NO_MATCH

	ADD R4,R4,#1		; mark letter as correctly guessed
	STR R4,R2,#0
	ADD R3,R3,#1		; and increment match count

GUESS_NO_MATCH
	ADD R1,R1,#1		; advance pointers and continue
	ADD R2,R2,#1
	BRnzp GUESS_CHECK_WORD

GUESS_END_WORD
	ADD R3,R3,#0		; any matches?
	BRz GUESS_MISSED

	NOT R3,R3		; subtract from remaining letter count
	ADD R3,R3,#1
	LD R0,REMAIN
	ADD R0,R0,R3
	ST R0,REMAIN
	BRp GUESS_AGAIN		; and guess again if necessary

	LD R0,POINTS		; word completed!  score 5 points
	ADD R0,R0,#5
	ST R0,POINTS

	JSR PRINT_STATUS	; show full word

	AND R0,R0,#0		; return R0=1 to indicate completion
	ADD R0,R0,#1
	BRnzp GUESS_DONE

GUESS_MISSED
	LD R0,POINTS		; lose a point
	ADD R0,R0,#-1
	ST R0,POINTS		; and must guess again...

GUESS_AGAIN
	AND R0,R0,#0		; word not completed

GUESS_DONE
	LD R7,GUESS_R7		; restore R7
	RET

GUESS_R7	.BLKW 1

ASCII_NEG_ESC	.FILL xFFE5	; negated ASCII ESCAPE character
ASCII_NEG_LC_A	.FILL xFF9F	; negated ASCII 'a'
NEG_NUM_LETTERS .FILL #-26

DICT_PTR	.FILL x7000

; ------------------------------------------------------------------------
; arithmetic routines that are useful when generating random numbers,
; including a logical shift right routine used to avoid the non-random 
; low-order bit of the simple generator and a MOD routine used to restrict
; the range of the random numbers as desired


; LOG_RIGHT_SHIFT -- logically shift R0 right by one bit (MSB <- 0)
;     INPUTS -- R0
;     OUTPUTS -- R0 shifted right by a bit
;     SIDE EFFECTS -- none

LOG_RIGHT_SHIFT
	ST R1,LRSHFT_R1		; save a copy of R1

	AND R0,R0,xFFFE		; set low bit to 0 (will become MSB)

	AND R1,R1,#0		; loop 15 times...
	ADD R1,R1,#15

LRSHFT_LOOP
	ADD R0,R0,#0		; rotate left (copy high bit to low bit)
	BRn LOW_BIT_IS_1
	ADD R0,R0,R0
	BRnzp LRSHFT_NEXT
LOW_BIT_IS_1
	ADD R0,R0,R0
	ADD R0,R0,1

LRSHFT_NEXT
	ADD R1,R1,#-1
	BRp LRSHFT_LOOP

	LD R1,LRSHFT_R1		; restore R1
	RET

LRSHFT_R1
	.BLKW 1



; MOD -- calculate the modulus of R0 relative to R1
;     INPUTS -- R0 and R1
;     OUTPUTS -- R0 as the original value MOD R1 (0 <= R0 < ABS (R1))
;     SIDE EFFECTS -- none


MOD	ST R1,MOD_R1		; save R1 through R4
	ST R2,MOD_R2
	ST R3,MOD_R3
	ST R4,MOD_R4

	ADD R1,R1,#0		; set R1 to its absolute value
	BRp MOD_R1_POS
	BRz MOD_ZERO_ERR
	NOT R1,R1
	ADD R1,R1,#1
	BRp MOD_R1_POS

	NOT R1,R1		; handle special case of R1=x8000
	AND R0,R0,R1		;    by ANDing with x7FFF
	BRnzp MOD_FOUND

MOD_ZERO_ERR
	ST R7,MOD_R7		; save R7 to allow trap below
	LEA R0,MSG_MOD		; handle special case of R1=x0000
	PUTS			;    by printing a warning
	AND R0,R0,#0		;    and returning 0
	LD R7,MOD_R7		; restore R7
	BRnzp MOD_FOUND

MOD_R1_POS
	ADD R0,R0,#0		; is R0 non-negative?
	BRzp MOD_POS_LOOP

; R0 is negative, so add multiples of R1 to make it positive

MOD_R0_NEG
	ADD R3,R1,#0		; start by adding R1
MOD_NEG_LOOP
	ADD R2,R3,#0
	ADD R4,R0,R2		; check if R2 is enough to make R0 >= 0
	BRzp FOUND_GOOD_ADD
	ADD R3,R2,R2		; if not, try doubling R2
	BRzp MOD_NEG_LOOP
	ADD R0,R0,R2		; if necessary, add in old R2 and start over
	BRnzp MOD_R0_NEG
FOUND_GOOD_ADD
	ADD R0,R0,R2		; add R2 to make R0 >= 0 

; R0 is non-negative, so subtract multiples of R1 to force it into the
;    correct range

MOD_POS_LOOP
	NOT R4,R1		; is R0 < R1 ?
	ADD R4,R4,#1
	ADD R4,R4,R0
	BRn MOD_FOUND

	; find biggest left shift of R1 that is both positive and <= R0
	ADD R2,R1,#0
MOD_SHIFT_LOOP
	ADD R3,R2,#0		; last value checked (R2) was acceptable
	ADD R2,R2,R2		; try multiplying again by 2
	BRn MOD_TOO_BIG
	NOT R4,R0		; is R2 < R0 ?
	ADD R4,R4,#1
	ADD R4,R4,R2
	BRnz MOD_SHIFT_LOOP

MOD_TOO_BIG			; subtract R3 (the biggest left shift of 
	NOT R4,R3		;    R1 that is <= R0) from R0
	ADD R4,R4,#1
	ADD R0,R0,R4

	BRnzp MOD_POS_LOOP	; start over with new value of R0

MOD_FOUND
	LD R1,MOD_R1		; restore R1 through R4
	LD R2,MOD_R2
	LD R3,MOD_R3
	LD R4,MOD_R4

	RET			; and return...

MOD_R1	.BLKW 1			; register storage for R1, R2, R3, and R4
MOD_R2	.BLKW 1
MOD_R3	.BLKW 1
MOD_R4	.BLKW 1
MOD_R7	.BLKW 1
MSG_MOD	.STRINGZ "\nWARNING: MOD called with R1=0\n"

; ------------------------------------------------------------------------
; the following subroutines provide random numbers using
; a simple pseudo-random number generator and a loop to
; wait for user input (the number of iterations becomes
; the seed for the generator).


; SRANDOM -- generate new random seed
;     INPUTS -- none
;     OUTPUTS -- none
;     SIDE EFFECTS -- changes random seed, destroys R0 and R1

SRANDOM	ST R7,RAND_R7		; save R7 to allow trap call below
	LEA R0,MSG_SRANDOM	; print a message
	PUTS
	LD R7,RAND_R7		; restore R7

	AND R0,R0,#0		; wait for a keystroke, counting iterations
SRANDOM_LOOP
	ADD R0,R0,#1
	LDI R1,SR_KBSR
	BRzp SRANDOM_LOOP
	
	LDI R1,SR_KBDR		; read (and ignore) the key

	ST R0,RAND_SEED		; store the newly generated random seed
	RET


; RANDOM -- generate random number
;     INPUTS -- none
;     OUTPUTS -- R0 is a random value
;     SIDE EFFECTS -- changes random seed

; function used is NEW = (27193 * OLD) + 35993   MOD 65536
;	low bit is right-shifted out before returning, since
;       it is not random (the rest are not too bad, at least by 
;       separation of order 2 in Knuth's methods...)

RANDOM	ST R1,RAND_R1		; save R1 and R7
	ST R7,RAND_R7
	LD R0,RAND_SEED
	ADD R1,R0,R0		; x 0002
	ADD R1,R1,R0		; x 0003
	ADD R1,R1,R1		; x 0006
	ADD R1,R1,R1		; x 000C
	ADD R1,R1,R0		; x 000D
	ADD R1,R1,R1		; x 001A
	ADD R1,R1,R1		; x 0034
	ADD R1,R1,R0		; x 0035
	ADD R1,R1,R1		; x 006A
	ADD R1,R1,R1		; x 00D4
	ADD R1,R1,R1		; x 01A8
	ADD R1,R1,R1		; x 0350
	ADD R1,R1,R0		; x 0351
	ADD R1,R1,R1		; x 06A2
	ADD R1,R1,R0		; x 06A3
	ADD R1,R1,R1		; x 0D46
	ADD R1,R1,R0		; x 0D47
	ADD R1,R1,R1		; x 1A8E
	ADD R1,R1,R1		; x 351C
	ADD R1,R1,R1		; x 6A38
	ADD R0,R1,R0		; x 6A39 = #27193
	LD R1,RAND_ADD
	ADD R0,R0,R1
	ST R0,RAND_SEED
	JSR LOG_RIGHT_SHIFT	; drop the low bit
	LD R1,RAND_R1		; restore R1 and R7
	LD R7,RAND_R7
	RET

; storage for SRANDOM and RANDOM

SR_KBSR	.FILL xFE00
SR_KBDR	.FILL xFE02
MSG_SRANDOM
	.STRINGZ "\nPlease press a key...\n"
RAND_R1	.BLKW 1
RAND_R7	.BLKW 1
RAND_SEED 
	.BLKW 1
RAND_ADD
	.FILL #35993

; ------------------------------------------------------------------------
; This subroutine builds a table of strings (starting addresses) from
; the list of strings below.  The number of entries in the table is
; stored at x7000, and the table starts at x7001.

; BUILD_DICT -- build the wordmaster dictionary from the strings below
;     INPUTS -- none
;     OUTPUTS -- none
;     SIDE EFFECTS -- builds a dictionary at x7000 from strings below,
;                     storing the number of strings (not to exceed 4000) 
;		      at x7000 and the string addresses as an array 
;		      starting at x7001.


BUILD_DICT
	ST R0,BUILD_R0		; save R0, R1, R2, R3, and R4 to stack
	ST R1,BUILD_R1
	ST R2,BUILD_R2
	ST R3,BUILD_R3
	ST R4,BUILD_R4

	LD R0,DICT_LOC		; R0 points to our dictionary
	ADD R0,R0,#1
	LEA R1,THE_WORDS	; R1 points to the list of strings
	AND R2,R2,#0		; R2 counts the words found

BUILD_LOOP
	LDR R3,R1,#0		; R3 is a character from the strings
	BRz BUILD_DONE

	STR R1,R0,#0		; write string pointer to dictionary
	ADD R0,R0,#1
	ADD R2,R2,#1

	AND R4,R4,#0		; count characters in string
BUILD_FIND_END
	ADD R1,R1,#1		; point R1 past end of current string
	ADD R4,R4,#1
	LDR R3,R1,#0
	BRnp BUILD_FIND_END
	ADD R1,R1,#1		; NUL doesn't count for string length (R4)

	LD R3,NEG_MAXLEN	; is string too long?
	ADD R4,R4,R3
	BRnz BUILD_STR_OK
	ADD R0,R0,#-1		; if so, discard silently
	ADD R2,R2,#-1

BUILD_STR_OK
	LD R3,NEG_LIMIT		; continue if we have fewer than maximum
	ADD R3,R3,R2		;    allowed string count 
	BRn BUILD_LOOP

BUILD_DONE
	STI R2,DICT_LOC		; write number of strings into dictionary

	LD R0,BUILD_R0		; restore R0, R1, R2, and R3, then return
	LD R1,BUILD_R1
	LD R2,BUILD_R2
	LD R3,BUILD_R3
	LD R4,BUILD_R4
	RET

DICT_LOC	.FILL x7000	; start of dictionary
NEG_LIMIT	.FILL -4000	; word count limit (negated)
NEG_MAXLEN	.FILL -40	; words are limited to 40 characters;
				;    matched to size of CORRECT array
BUILD_R0	.BLKW 1		; storage to save registers used
BUILD_R1	.BLKW 1
BUILD_R2	.BLKW 1
BUILD_R3	.BLKW 1
BUILD_R4	.BLKW 1
	

; IMPORTANT NOTE: No string can have more than 40 characters!

THE_WORDS	; Apologies to Percy, ee, Bill, Tom, etc.
		; It's harder to think than to recite!

	.STRINGZ "apple"
	.STRINGZ "banana"
	.STRINGZ "cucumber"
	.STRINGZ "doppleganger"
	.STRINGZ "eggplant"
	.STRINGZ "ferocious"
	.STRINGZ "gangrene"
	.STRINGZ "hierarchical"
	.STRINGZ "idiomatic"
	.STRINGZ "juice"
	.STRINGZ "kangaroo"
	.STRINGZ "leopard"
	.STRINGZ "mongoose"
	.STRINGZ "nougat"
	.STRINGZ "orange"
	.STRINGZ "perspicacious"
	.STRINGZ "quality"
	.STRINGZ "reality"
	.STRINGZ "serenity"
	.STRINGZ "testimony"
	.STRINGZ "umbrella"
	.STRINGZ "voice"
	.STRINGZ "watershed"
	.STRINGZ "xylophone"
	.STRINGZ "yellow"
	.STRINGZ "yesterday"
	.STRINGZ "zinger"
	.STRINGZ "zymurgy"
	.STRINGZ "anticlimactic"
	.STRINGZ "imperial"
	.STRINGZ "persimmon"
	.STRINGZ "rutebaga"
	.STRINGZ "watermelon"
	.STRINGZ "yam"
	.STRINGZ "pester"
	.STRINGZ "poster"
	.STRINGZ "pistol"
	.STRINGZ "postal"
	.STRINGZ "pallet"
	.STRINGZ "pumpkin"
	.STRINGZ "red"
	.STRINGZ "hardly"
	.STRINGZ "garbled"
	.STRINGZ "master"
	.STRINGZ "once"
	.STRINGZ "zinger"
	.STRINGZ "which"
	.STRINGZ "much"
	.STRINGZ "wind"
	.STRINGZ "gave"
	.STRINGZ "truth"
	.STRINGZ "summer"
	.STRINGZ "bloodied"
	.STRINGZ "dizzying"
	.STRINGZ "leaves"
	.STRINGZ "sun"
	.STRINGZ "yanked"
	.STRINGZ "immortal"
	.STRINGZ "stars"
	.STRINGZ "awry"
	.STRINGZ "trunkless"
	.STRINGZ "antique"
	.STRINGZ "vast"
	.STRINGZ "legs"
	.STRINGZ "stone"
	.STRINGZ "stand"
	.STRINGZ "desert"
	.STRINGZ "dessert"
	.STRINGZ "sand"
	.STRINGZ "visage"
	.STRINGZ "wrinkled"
	.STRINGZ "command"
	.STRINGZ "sneer"
	.STRINGZ "sculptor"
	.STRINGZ "passions"
	.STRINGZ "remain"
	.STRINGZ "stamped"
	.STRINGZ "lifeless"
	.STRINGZ "things"
	.STRINGZ "bare"
	.STRINGZ "nothing"
	.STRINGZ "beside"
	.STRINGZ "colossal"
	.STRINGZ "boundless"
	.STRINGZ "lone"
	.STRINGZ "level"
	.STRINGZ "stretch"
	.STRINGZ "compare"
	.STRINGZ "lovely"
	.STRINGZ "temperate"
	.STRINGZ "rough"
	.STRINGZ "shake"
	.STRINGZ "darling"
	.STRINGZ "lease"
	.STRINGZ "short"
	.STRINGZ "date"
	.STRINGZ "sometimes"
	.STRINGZ "eye"
	.STRINGZ "shine"
	.STRINGZ "often"
	.STRINGZ "gold"
	.STRINGZ "complexion"
	.STRINGZ "dimmed"
	.STRINGZ "declines"
	.STRINGZ "fair"
	.STRINGZ "chance"
	.STRINGZ "nature"
	.STRINGZ "untrimmed"
	.STRINGZ "course"
	.STRINGZ "eternal"
	.STRINGZ "fade"
	.STRINGZ "possession"
	.STRINGZ "obsession"
	.STRINGZ "poesy"
	.STRINGZ "poems"
	.STRINGZ "poetry"
	.STRINGZ "brag"
	.STRINGZ "breathe"
	.STRINGZ "long"
	.STRINGZ "this"
	.STRINGZ "life"
	.STRINGZ "throw"
	.STRINGZ "drummer"
	.STRINGZ "coffin"
	.STRINGZ "found"
	.STRINGZ "around"
	.STRINGZ "mound"
	.STRINGZ "sound"
	.STRINGZ "pound"
	.STRINGZ "kopje"
	.STRINGZ "veldt"
	.STRINGZ "constellations"
	.STRINGZ "foreign"
	.STRINGZ "night"
	.STRINGZ "never"
	.STRINGZ "knew"
	.STRINGZ "home"
	.STRINGZ "meaning"
	.STRINGZ "gloam"
	.STRINGZ "portion"
	.STRINGZ "plain"
	.STRINGZ "forever"
	.STRINGZ "homely"
	.STRINGZ "breast"
	.STRINGZ "brain"
	.STRINGZ "grown"
	.STRINGZ "tree"
	.STRINGZ "happy"
	.STRINGZ "despair"
	.STRINGZ "repair"
	.STRINGZ "fortune"
	.STRINGZ "beweep"
	.STRINGZ "outcast"
	.STRINGZ "state"
	.STRINGZ "trouble"
	.STRINGZ "bootless"
	.STRINGZ "self"
	.STRINGZ "curse"
	.STRINGZ "wishing"
	.STRINGZ "like"
	.STRINGZ "rich"
	.STRINGZ "hope"
	.STRINGZ "featured"
	.STRINGZ "desiring"
	.STRINGZ "scope"
	.STRINGZ "despising"
	.STRINGZ "lark"
	.STRINGZ "break"
	.STRINGZ "day"
	.STRINGZ "arising"
	.STRINGZ "rhythm"
	.STRINGZ ""			; empty string marks end

; ------------------------------------------------------------------------

	.END
