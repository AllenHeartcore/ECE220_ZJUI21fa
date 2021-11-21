; ------------------------------------------------------------------------
;
; flowers
;
; A game similar in content to some game you may have seen, but without 
; all the nasty side effects of winning or losing.  Instead, you try to
; find flowers in each others' gardens.
;

	.ORIG x3000

	LD R0,RSEED		; seed the random number generator
	TRAP x31

	JSR CLEAR_BOARDS	; empty both boards and scores
	LD R2,BOARDREF		; create the player's board
	JSR RAND_BOARD
	LD R2,BOARDREF		; create the computer's board
	ADD R2,R2,#10
	JSR RAND_BOARD

PLAY_PLAY
	; now it's time to play the game--the player goes first
	JSR PLAYER_TURN
	ADD R0,R0,#0
	BRz COMP_PLAY

	; the player has won!
	LEA R0,P_WINS
	PUTS

	IN

	HALT

COMP_PLAY
	JSR COMPUTER_TURN
	ADD R0,R0,#0
	BRz PLAY_PLAY

	JSR PRINT_BOARDS
	LEA R0,C_WINS
	PUTS

	IN

	HALT

RSEED	.FILL xD7D9

BOARDREF .FILL BOARDS
P_WINS	 .STRINGZ "\nCongratulations!  You found all of my flowers!\n"
C_WINS	 .STRINGZ "\nBetter luck next time!\n"


;   0 1 2 3 4 5 6 7 8 9       0 1 2 3 4 5 6 7 8 9
;  +-+-+-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+-+-+
;A | | | | | | | | | | |     | | | | | | | | | | | A
;  +-+-+-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+-+-+
;B | | | | | | | | | | |     | | | | | | | | | | | B
;  +-+-+-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+-+-+

; R0 - temporary/printing
; R1 - board data pointer
; R2 - row number (0 to 9)
; R3 - column number (not used directly; counts up to 10 then back down)
; R4 - temporary

PRINT_BOARDS
	ST R7,PB_R7	; save R7

	LEA R1,BOARDS	; point R1 to the board data

	LEA R0,PB_S1	; print the first lines of the board
	PUTS

	AND R2,R2,#0	; loop over rows (R2 is the row number)
PB_LOOP
	LD R0,PB_RLET	; print the row letter at the start
	ADD R0,R0,R2
	OUT
	LD R0,PB_SP
	OUT

	AND R3,R3,#0	; loop over columns in the player's board
PBR1_LOOP		;    (R3 is the column number)
	LD R0,PB_BAR
	OUT
	LDR R0,R1,#0	; find correct character for board location
	LEA R4,PLAY_CHAR
	ADD R4,R4,R0
	LDR R0,R4,#0
	OUT
	ADD R1,R1,#1	; point to next board location
	ADD R3,R3,#1	; increment column
	ADD R0,R3,#-10	; done yet?
	BRn PBR1_LOOP

	LEA R0,PB_S2	; print inter-board string
	PUTS

PBR2_LOOP		; loop over columns in the computer's board
			;    (R3 is 10 - column number)
	LDR R0,R1,#0	; find correct character for board location
	LEA R4,COMP_CHAR
	ADD R4,R4,R0
	LDR R0,R4,#0
	OUT
	LD R0,PB_BAR
	OUT
	ADD R1,R1,#1	; point to next board location
	ADD R3,R3,#-1	; decrement column
	BRp PBR2_LOOP	; done yet?

	LD R0,PB_SP	; print the row letter at the end, too
	OUT
	LD R0,PB_RLET
	ADD R0,R0,R2
	OUT
	LEA R0,PB_S3	; print the row separator line
	PUTS

	ADD R2,R2,#1	; increment the row
	ADD R0,R2,#-10	; done with all rows yet?
	BRn PB_LOOP

	LD R7,PB_R7	; restore R7 and return
	RET

PB_R7   .BLKW #1
PB_S1   .STRINGZ "

   0 1 2 3 4 5 6 7 8 9       0 1 2 3 4 5 6 7 8 9
  +-+-+-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+-+-+
"
PB_RLET	.FILL x41  ; row letters start from 'A'
PB_SP	.FILL x20  ; space separates row letters from board
PB_BAR	.FILL x7C  ; vertical bars ('|') separate board spaces
PB_S2   .STRINGZ "|     |"
PB_S3	.STRINGZ "
  +-+-+-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+-+-+
"

; board contents
;    bit 3 (value 8) = space was searched
;    bits 2-0 = flower # present
;           0 = none
;           1 = E elyssium (2 spaces)
;           2 = D dandelion (3 spaces)
;           3 = C carnation (3 spaces)
;           4 = T tulip (4 spaces)
;           5 = R rose (5 spaces)
;
PLAY_CHAR  .STRINGZ " EDCTR??.XXXXX??"
;COMP_CHAR  .STRINGZ "        .*****??"
COMP_CHAR  .STRINGZ "        .*****??"
BOARDS     .BLKW #200   ; two 10x10 boards logically laid out as 10 rows of 20
BOARD_SIZE .FILL #200

; fill both boards with empty spaces (zeroes)

CLEAR_BOARDS
	LEA R0,BOARDS
	LD R1,BOARD_SIZE
	AND R2,R2,#0
CB_LOOP 
	STR R2,R0,#0
	ADD R0,R0,#1
	ADD R1,R1,#-1
	BRp CB_LOOP

	AND R0,R0,#0
	LEA R1,FLOWER_SIZE
	LEA R2,P_REMAIN
	LEA R3,C_REMAIN
CB_TAB_LOOP
	LDR R5,R1,#1
	STR R5,R2,#1
	STR R5,R3,#1
	ADD R0,R0,#1
	ADD R1,R1,#1
	ADD R2,R2,#1
	ADD R3,R3,#1
	ADD R5,R0,#-5
	BRn CB_TAB_LOOP
	ST R0,P_REMAIN
	ST R0,C_REMAIN
	RET
	

; R2 = board data pointer (passed as parameter)
; R3 = flower number
; R4 = negative flower size
; R5 = 11 - flower size (used to limit flower placement)

RAND_BOARD
	ST R7,RB_R7
	ST R2,RB_BPTR

	AND R3,R3,#0
RB_LOOP	ADD R3,R3,#1
	ADD R0,R3,#-6
	BRzp RB_DONE

	LEA R4,FLOWER_SIZE
	ADD R4,R4,R3
	LDR R4,R4,#0
	NOT R4,R4
	ADD R4,R4,#1
	ADD R5,R4,#11	; R5 <- 11 - size (used to limit flower placement)

RB_CHOOSE_LOC
	JSR RANDOM	; pick an X value and add it to the board pointer
	ADD R1,R5,#0
	JSR MOD
	ADD R2,R2,R0

	JSR RANDOM	; pick an Y value and add it (x20) to the board pointer
	ADD R1,R5,#0
	JSR MOD
	ADD R1,R0,R0	; R1 = (y value)x2
	ADD R1,R1,R1	; R1 = (y value)x4
	ADD R0,R0,R1	; R0 = (y value)x5
	ADD R0,R0,R0	; R0 = (y value)x10
	ADD R0,R0,R0	; R0 = (y value)x20
	ADD R2,R2,R0

	JSR RANDOM	; left-right or up-down?
	AND R0,R0,#1
	BRz RB_UP_DOWN

	AND R1,R1,#0
RB_TEST_LR
	LDR R0,R2,#0
	BRnp RB_DISCARD
	ADD R2,R2,#1
	ADD R1,R1,#1
	ADD R0,R1,R4
	BRn RB_TEST_LR
RB_FILL_LR
	ADD R2,R2,#-1
	STR R3,R2,#0
	ADD R1,R1,#-1
	BRp RB_FILL_LR
	LD R2,RB_BPTR
	BRnzp RB_LOOP

RB_UP_DOWN
	AND R1,R1,#0
RB_TEST_UD
	LDR R0,R2,#0
	BRnp RB_DISCARD
	ADD R2,R2,#10
	ADD R2,R2,#10
	ADD R1,R1,#1
	ADD R0,R1,R4
	BRn RB_TEST_UD
RB_FILL_UD
	ADD R2,R2,#-10
	ADD R2,R2,#-10
	STR R3,R2,#0
	ADD R1,R1,#-1
	BRp RB_FILL_UD
	LD R2,RB_BPTR
	BRnzp RB_LOOP

RB_DISCARD	
	LD R2,RB_BPTR
	BRnzp RB_CHOOSE_LOC

RB_DONE
	LD R7,RB_R7
	RET

RB_R7	.BLKW #1
RB_BPTR	.BLKW #1


FLOWER_SIZE
	.BLKW #1	; index starts at 1
	.FILL #2	; elyssium (#1)
	.FILL #3	; dandelion (#2)
	.FILL #3	; carnation (#3)
	.FILL #4	; tulip (#4)
	.FILL #5	; rose (#5)

P_REMAIN .BLKW #6	; index 0 is flowers remaining
C_REMAIN .BLKW #6	; index 0 is flowers remaining

FLOWER_NAME
	.BLKW #1	; index starts at 1
	.FILL F1NAME	; elyssium (#1)
	.FILL F2NAME	; dandelion (#2)
	.FILL F3NAME	; carnation (#3)
	.FILL F4NAME	; tulip (#4)
	.FILL F5NAME	; rose (#5)
F1NAME	.STRINGZ "elyssium!"
F2NAME	.STRINGZ "dandelion!"
F3NAME	.STRINGZ "carnation!"
F4NAME	.STRINGZ "tulip!"
F5NAME	.STRINGZ "rose!"


PLAYER_TURN
	ST R7,PT_R7

PT_PICK
	JSR PRINT_BOARDS
	LEA R0,PT_R7
	PUTS

PT_READ_ROW
	GETC
	LD R1,PT_NEGA
	ADD R1,R0,R1
	BRn PT_READ_ROW
	ADD R0,R1,#-10
	BRzp PT_READ_ROW
	LD R0,PT_POSA
	ADD R0,R0,R1
	OUT
	ADD R3,R1,R1
	ADD R3,R3,R3
	ADD R3,R3,R1
	ADD R3,R3,R3
	ADD R3,R3,R3

PT_READ_COL
	GETC
	LD R1,PT_NEG0
	ADD R1,R0,R1
	BRn PT_READ_COL
	ADD R2,R1,#-10
	BRzp PT_READ_COL
	OUT

	LD R2,PT_BREF
	ADD R2,R2,R1
	ADD R2,R2,R3
	LDR R3,R2,#10
	AND R0,R3,#8
	BRz PT_GOOD

	LEA R0,PT_USED
	PUTS
	BRnzp PT_PICK

PT_GOOD	ADD R0,R3,#8
	STR R0,R2,#10

	ADD R3,R3,#0
	BRz PT_MISS
	LD R1,PT_CREF
	ADD R2,R1,R3
	LDR R0,R2,#0
	ADD R0,R0,#-1
	STR R0,R2,#0
	BRp PT_MISS
	LEA R0,PT_FULL
	PUTS
	LD R0,PT_NREF
	ADD R0,R0,R3
	LDR R0,R0,#0
	PUTS

	LDR R0,R1,#0
	ADD R0,R0,#-1
	STR R0,R1,#0
	BRp PT_MISS

	ADD R0,R0,#1
	BRnzp PT_DONE

PT_MISS	AND R0,R0,#0
PT_DONE	LD R7,PT_R7
	RET

PT_R7	  .BLKW #1
PT_PROMPT .STRINGZ "\nWhere will you look (row followed by column)? "
PT_NEGA	  .FILL xFF9F	; negative 'a'
PT_POSA   .FILL x41	; 'A'
PT_NEG0	  .FILL xFFD0	; negative '0'
PT_BREF	  .FILL BOARDS
PT_CREF	  .FILL C_REMAIN
PT_NREF	  .FILL FLOWER_NAME
PT_USED	  .STRINGZ "\nYou've already looked there!\n"
PT_FULL	  .STRINGZ "\nYou found my "

COMPUTER_TURN
	ST R7,CT_R7

	AND R5,R5,#0
	ADD R5,R5,#10

CT_CHOOSE_LOC
	LD R2,PT_BREF

	JSR RANDOM	; pick an X value and add it to the board pointer
	ADD R1,R5,#0
	JSR MOD
	ADD R2,R2,R0

	JSR RANDOM	; pick an Y value and add it (x20) to the board pointer
	ADD R1,R5,#0
	JSR MOD
	ADD R1,R0,R0	; R1 = (y value)x2
	ADD R1,R1,R1	; R1 = (y value)x4
	ADD R0,R0,R1	; R0 = (y value)x5
	ADD R0,R0,R0	; R0 = (y value)x10
	ADD R0,R0,R0	; R0 = (y value)x20
	ADD R2,R2,R0

	LDR R3,R2,#0
	AND R0,R3,#8
	BRnp CT_CHOOSE_LOC

	ADD R0,R3,#8
	STR R0,R2,#0
	
	ADD R3,R3,#0
	BRz CT_MISS
	LD R1,CT_PREF
	ADD R2,R1,R3
	LDR R0,R2,#0
	ADD R0,R0,#-1
	STR R0,R2,#0
	BRp CT_MISS
	LEA R0,CT_FULL
	PUTS
	LD R0,PT_NREF
	ADD R0,R0,R3
	LDR R0,R0,#0
	PUTS

	LDR R0,R1,#0
	ADD R0,R0,#-1
	STR R0,R1,#0
	BRp CT_MISS

	ADD R0,R0,#1
	BRnzp CT_DONE

CT_MISS	AND R0,R0,#0
CT_DONE	LD R7,CT_R7
	RET

CT_R7	.BLKW #1
CT_PREF	.FILL P_REMAIN
CT_FULL	.STRINGZ "\nAha!  I've found your "

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

RANDOM	ST R7,RAND_R7
	TRAP x30
	LD R7,RAND_R7
	RET

;RANDOM	ST R1,RAND_R1		; save R1 and R7
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

	.END
