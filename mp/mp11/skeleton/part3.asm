
;---------------------------------------------------------------------------
; STUDENT CODE ENDS HERE
;---------------------------------------------------------------------------

	LDR	R0,R6,#0	; restore R0...R3
	LDR	R1,R6,#1
	LDR	R2,R6,#2
	LDR	R3,R6,#3

	ADD	R6,R5,#1	; pop off local variables
	LDR	R5,R6,#0
	LDR	R7,R6,#1
	ADD	R6,R6,#2	; leave return value on stack
	RET
	
;---------------------------------------------------------------------------
; C library routines
;---------------------------------------------------------------------------

; assembly routines in this library
;   MULTIPLY (R0 <- R0 * R1)
;   DIVIDE   (R0 <- R0 / R1, rounded toward 0)
;   MODULUS  (R0 <- R0 MOD R1, using C's definition)

; routines with C interfaces in this library
;   int PRINTF (const char* fmt, ...);
;   int SCANF (const char* fmt, ...);
;   void SRAND (int new_seed);
;   int RAND ();
; NOTES: 
;    - ALL C ROUTINES LEAVE A RETURN VALUE LOCATION ON THE STACK, EVEN
;      IF THEY PRODUCE NO RETURN VALUE!
;    - PRINTF and SCANF only handle %d, %%, \n, \\, and normal characters
;

; INTERNAL routines (you should not call them)
;   LOG_RIGHT_SHIFT
;   PRINT_NUM
;   LOAD_FORMAT
;

;---------------------------------------------------------------------------

; MULTIPLY -- calculate R0 * R1
;     INPUTS -- R0 and R1
;     OUTPUTS -- R0 is the product
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: the calling convention here is NOT for use directly by C
;
MULTIPLY
	ADD R6,R6,#-3	; save R1, R2, and R3
	STR R1,R6,#0
	STR R2,R6,#1
	STR R3,R6,#2

	AND R2,R2,#0	; number of negative operands

	ADD R1,R1,#0	; set R1 to its absolute value
	BRzp MULT_R1_NON_NEG
	NOT R1,R1
	ADD R1,R1,#1
	ADD R2,R2,#1
MULT_R1_NON_NEG

	AND R3,R3,#0
MULT_LOOP
	ADD R1,R1,#0
	BRz MULT_FINISH
	ADD R3,R3,R0
	ADD R1,R1,#-1
	BRnzp MULT_LOOP

MULT_FINISH
	ADD R0,R3,#0	; move result into R0

	AND R2,R2,#1	; negate answer?
	BRz MULT_DONE
	NOT R0,R0
	ADD R0,R0,#1

MULT_DONE
	LDR R1,R6,#0	; restore R1, R2, and R3
	LDR R2,R6,#1
	LDR R3,R6,#2
	ADD R6,R6,#3
	RET


; DIVIDE -- calculate R0 / R1 (rounded toward zero)
;     INPUTS -- R0 and R1
;     OUTPUTS -- R0 is the quotient
;     SIDE EFFECTS -- uses stack to save registers; may print divide by
;                     zero error
;     NOTE: the calling convention here is NOT for use directly by C
;
DIVIDE	ADD R6,R6,#-4	; save R1, R2, R3, and R7
	STR R1,R6,#0
	STR R2,R6,#1
	STR R3,R6,#2
	STR R7,R6,#3

	AND R2,R2,#0	; number of negative operands
	ADD R2,R2,#1

	ADD R1,R1,#0	; set R1 to its negative absolute value
	BRn DIV_R1_NEG
	BRp DIV_R1_POS
	LEA R0,MSG_DIV
	PUTS
	AND R0,R0,#0
	BRnzp DIV_DONE
DIV_R1_POS
	NOT R1,R1
	ADD R1,R1,#1
	ADD R2,R2,#-1
DIV_R1_NEG

	ADD R0,R0,#0	; set R0 to its absolute value
	BRzp DIV_R0_NON_NEG
	NOT R0,R0
	ADD R0,R0,#1
	ADD R2,R2,#1
DIV_R0_NON_NEG

	AND R3,R3,#0
DIV_LOOP
	ADD R0,R0,R1
	BRn DIV_FINISH
	ADD R3,R3,#1
	BRnzp DIV_LOOP
DIV_FINISH
	ADD R0,R3,#0	; move result into R0

	AND R2,R2,#1	; negate answer?
	BRz DIV_DONE
	NOT R0,R0
	ADD R0,R0,#1

DIV_DONE
	LDR R1,R6,#0	; restore R1, R2, R3, and R7
	LDR R2,R6,#1
	LDR R3,R6,#2
	LDR R7,R6,#3
	ADD R6,R6,#4
	RET

MSG_DIV	.STRINGZ "\nDIVIDE BY ZERO\n"



; MODULUS -- calculate R0 MOD R1 (defined in C as R0 - (R0 / R1) * R1)
;     INPUTS -- R0 and R1
;     OUTPUTS -- R0 is the modulus
;     SIDE EFFECTS -- uses stack to save registers; may print divide by
;                     zero error
;     NOTE: the calling convention here is NOT for use directly by C
;
MODULUS	ADD R6,R6,#-3	; save R0, R1, and R7
	STR R0,R6,#0
	STR R1,R6,#1
	STR R7,R6,#2

	JSR DIVIDE	; R0 = R0 / R1
	JSR MULTIPLY	; R0 = (R0 / R1) * R1
	NOT R1,R0	; negate it
	ADD R1,R1,#1
	LDR R0,R6,#0	; add to original R0
	ADD R0,R0,R1

	LDR R1,R6,#1	; restore R1 and R7
	LDR R7,R6,#2
	ADD R6,R6,#3
	RET


; SRAND -- set random number generation seed
;     INPUTS -- new seed (on top of stack)
;     OUTPUTS -- one (meaningless) location left on top of stack
;     SIDE EFFECTS -- changes random seed
;     NOTE: call as a C function
;
SRAND	ADD R6,R6,#-1		; save R0
	STR R0,R6,#0
	LDR R0,R6,#1
	ST R0,RAND_SEED
	LDR R0,R6,#0		; restore R0
	RET


; LOG_RIGHT_SHIFT -- logically shift R0 right by one bit (MSB <- 0)
;     INPUTS -- R0
;     OUTPUTS -- R0 shifted right by a bit
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: the calling convention here is NOT for use directly by C
;
LOG_RIGHT_SHIFT
	ADD R6,R6,#-1		; save R1
	STR R1,R6,#0

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

	LDR R1,R6,#0		; restore R1
	ADD R6,R6,#1
	RET


; RAND -- generate random number using the function
;             NEW = (27193 * OLD) + 35993   MOD 65536
;	  the low bit is right-shifted out before returning, since
;         it is not random (the rest are not too bad, at least by 
;         separation of order 2 in Knuth's methods...)
;     INPUTS -- none
;     OUTPUTS -- random value left on top of stack (return value)
;     SIDE EFFECTS -- changes random seed
;     NOTE: call as a C function
;
RAND	ADD R6,R6,#-3		; save R0, R1, and R7
	STR R0,R6,#0
	STR R1,R6,#1
	STR R7,R6,#2
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
	LDR R7,R6,#2		; restore R7
	STR R0,R6,#2		; save return value onto stack
	LDR R0,R6,#0		; restore R0 and R1 
	LDR R1,R6,#1
	ADD R6,R6,#2
	RET

; storage for SRAND and RAND

RAND_SEED 
	.BLKW 1
RAND_ADD
	.FILL #35993


; PRINT_NUM -- print a number in decimal to the monitor (based on code 
;              incorporated as TRAP x26 for MP2 in the Spring 2004 
;              semester of ECE190)
;     INPUTS -- R0 is the number to be printed
;     OUTPUTS -- R0 is the number of characters printed
;     SIDE EFFECTS -- none
;     NOTE: the calling convention here is NOT for use directly by C
;
; The basic strategy is to handle the sign first, then to loop over place
; values starting from 10,000 down to 10.  Place values are subtracted
; repeatedly to calculate each digit, then digits are printed, with 
; leading zeroes omitted.

; R0 is the current digit (calculated in the inner loop)
; R1 points to table of negative digit place values
; R2 holds current digit's place value, again negative
; R3 is the remaining value after removing the previous digit
; R4 is a temporary
; R5 holds the ASCII value '0'
; R6 is a marker used to avoid leading zeroes

PRINT_NUM
	ST R1,PN_SAVE_R1	; callee saves registers
	ST R2,PN_SAVE_R2
	ST R3,PN_SAVE_R3
	ST R4,PN_SAVE_R4
	ST R5,PN_SAVE_R5
	ST R6,PN_SAVE_R6
	ST R7,PN_SAVE_R7

	AND R3,R0,#0		; initialize number of characters printed
	ST R3,PN_PRINTED

	ADD R3,R0,#0		; move to R3 and check for negative value
	BRzp PN_NON_NEG
	LD R0,PN_MINUS		; if negative, print a minus sign
	OUT
	LD R0,PN_PRINTED	; add one to printed characters count
	ADD R0,R0,#1
	ST R0,PN_PRINTED
	NOT R3,R3		; and replace R0 with its absolute value
	ADD R3,R3,#1		; (-32768 will be handled correctly, too)
PN_NON_NEG
	
	LEA R1,PN_SUB		; initialize pointer to place value table
	LD R5,PN_ASC_ZERO	; initialize register with ASCII '0'
	AND R6,R6,#0		; skip leading zeroes
PN_LOOP
	LDR R2,R1,#0		; load digit place value from table
	BRz PN_LAST_DIGIT	; end of table?
	AND R0,R0,#0		; start current digit at 0 (count ADDs)
PN_DIG_LOOP			; loop to determine digit value
	ADD R4,R3,R2		; subtract place value once
	BRn PN_DIG_DONE		; done?
	ADD R3,R4,#0		; no, so copy to remaining value
	ADD R0,R0,#1		;   and increment digit
	BRnzp PN_DIG_LOOP
PN_DIG_DONE
	ADD R4,R0,R6		; do not print leading zeroes
	BRz PN_NO_PRINT
	ADD R0,R0,R5		; print current digit
	OUT
	LD R0,PN_PRINTED	; add one to printed characters count
	ADD R0,R0,#1
	ST R0,PN_PRINTED
	ADD R6,R6,#1		; always print subsequent digits, even zeroes
PN_NO_PRINT
	ADD R1,R1,#1		; point to next place value
	BRnzp PN_LOOP		; loop back for next digit
PN_LAST_DIGIT
	ADD R0,R3,R5		; always print last digit
	OUT
	LD R0,PN_PRINTED	; add one to printed characters count
	ADD R0,R0,#1

	LD R1,PN_SAVE_R1	; restore original register values
	LD R2,PN_SAVE_R2
	LD R3,PN_SAVE_R3
	LD R4,PN_SAVE_R4
	LD R5,PN_SAVE_R5
	LD R6,PN_SAVE_R6
	LD R7,PN_SAVE_R7
	RET

PN_SAVE_R1 .BLKW 1		; space for caller's register values
PN_SAVE_R2 .BLKW 1
PN_SAVE_R3 .BLKW 1
PN_SAVE_R4 .BLKW 1
PN_SAVE_R5 .BLKW 1
PN_SAVE_R6 .BLKW 1
PN_SAVE_R7 .BLKW 1
PN_PRINTED .BLKW 1

PN_SUB  .FILL #-10000		; table of place values
	.FILL #-1000
	.FILL #-100
	.FILL #-10
	.FILL #0

PN_ASC_ZERO .FILL x30		; '0'
PN_MINUS    .FILL x2D		; '-'


; LOAD_FORMAT -- load a character from a format string (for PRINTF or
;		 SCANF), translating escape sequences (-1 for %d)
;                and advancing the string pointer appropriately
;     INPUTS -- R1 is the format string pointer
;     OUTPUTS -- R0 is the next character (-1 for %d)
;                R1 is advanced either one or two locations
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: the calling convention here is NOT for use directly by C
;
LOAD_FORMAT
	ADD R6,R6,#-2	; save R2 and R3
	STR R2,R6,#0
	STR R3,R6,#1
	LDR R0,R1,#0
	LD R2,LDF_TEST_1
	ADD R3,R0,R2
	BRnp LDF_NOT_PCT
	LDR R0,R1,#1
	ADD R2,R0,R2
	BRnp LDF_CHECK_D
	ADD R1,R1,#1
LDF_BAD_PCT
	LDR R0,R1,#0
	BRnzp LDF_DONE
LDF_CHECK_D
	LD R2,LDF_TEST_2
	ADD R0,R0,R2
	BRnp LDF_BAD_PCT
	AND R0,R0,#0
	ADD R0,R0,#-1
	ADD R1,R1,#1
	BRnzp LDF_DONE
LDF_NOT_PCT
	LD R2,LDF_TEST_3
	ADD R3,R0,R2
	BRnp LDF_DONE
	LDR R0,R1,#1
	ADD R2,R0,R2
	BRnp LDF_CHECK_N
	ADD R1,R1,#1
LDF_BAD_BS
	LDR R0,R1,#0
	BRnzp LDF_DONE
LDF_CHECK_N
	LD R2,LDF_TEST_4
	ADD R0,R0,R2
	BRnp LDF_BAD_BS
	AND R0,R0,#0
	ADD R0,R0,#10
	ADD R1,R1,#1
LDF_DONE
	ADD R1,R1,#1	; default string pointer advance
	LDR R2,R6,#0	; restore R2 and R3
	LDR R3,R6,#1
	ADD R6,R6,#2
	RET

LDF_TEST_1 	.FILL xFFDB	; -'%'
LDF_TEST_2	.FILL xFF9C	; -'d'
LDF_TEST_3	.FILL xFFA4	; -'\\'
LDF_TEST_4	.FILL xFF92	; -'n'


; PRINTF -- print formatted data
;     INPUTS -- format string followed by arguments
;     OUTPUTS -- number of characters printed left on top of stack 
;                (return value)
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: call as a C function
;
; R0 holds the character to print
; R1 is the format string pointer
; R2 points to the next argument
; R3 is the number of characters printed so far
;
PRINTF	ADD R6,R6,#-5		; save R0, R1, R2, R3, and R7
	STR R0,R6,#0
	STR R1,R6,#1
	STR R2,R6,#2
	STR R3,R6,#3
	STR R7,R6,#4
	LDR R1,R6,#5
	ADD R2,R6,#6
	AND R3,R3,#0
PR_LOOP	JSR LOAD_FORMAT
	ADD R0,R0,#0
	BRz PR_DONE
	BRp PR_REG
	LDR R0,R2,#0
	ADD R2,R2,#1
	JSR PRINT_NUM
	ADD R3,R3,R0
	BRnzp PR_LOOP
PR_REG	OUT
	ADD R3,R3,#1
	BRnzp PR_LOOP
PR_DONE	LDR R7,R6,#4		; restore R7
	STR R3,R6,#4		; save return value
	LDR R0,R6,#0		; restore R0, R1, R2, and R3
	LDR R1,R6,#1
	LDR R2,R6,#2
	LDR R3,R6,#3
	ADD R6,R6,#4
	RET


; BUF_GETC -- read a character from the keyboard, with preference for
;                a character previously read but buffered (in INBUF)
;     INPUTS -- none
;     OUTPUTS -- R4 holds the character
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: the calling convention here is NOT for use directly by C
;
BUF_GETC
	ADD R6,R6,#-2
	STR R0,R6,#0
	STR R7,R6,#1
	LD R4,INBUF
	BRnp BGC_OLD
	GETC
	OUT
	ADD R4,R0,#0
	BRnzp BGC_DONE
BGC_OLD	LD R0,INBUF2
        ST R0,INBUF
	AND R0,R0,#0
	ST R0,INBUF
BGC_DONE
	LDR R0,R6,#0
	LDR R7,R6,#1
	ADD R6,R6,#2
	RET

; BUF_UNGETC -- push a character back into the input buffer
;     INPUTS -- R4 holds the character
;     OUTPUTS -- none
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: the calling convention here is NOT for use directly by C
;
BUF_UNGETC
	ADD R6,R6,#-1
	STR R0,R6,#0
	LD R0,INBUF
	ST R0,INBUF2
	ST R4,INBUF
	LDR R0,R6,#0
	ADD R6,R6,#1
	RET

; READ_NUM -- read a decimal number from the keyboard, starting with
;             a character previously read but buffered (in INBUF) if necessary;
;             skip white space before the first digit; terminate on non-digit
;             (after first digit); buffer character that causes termination;
;             ignore overflow
;             (this code based on readnumsub.asm code from 190 materials)
;     INPUTS -- none
;     OUTPUTS -- R4 holds the number typed in; R0 holds 1 if number was typed,
;                or 0 if not
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: the calling convention here is NOT for use directly by C
;

; R0 is used as a temporary register
; R1 holds the current value of the number being input
; R2 holds the additive inverse of ASCII '0' (0xFFD0)
; R3 is used as a temporary register
; R4 holds the value of the last key pressed
; R5 marks whether a digit has been seen (positive), just a negative sign (-),
;    or nothing has been seen (0) yet

READ_NUM
	ADD R6,R6,#-5		; save R1, R2, R3, R5, and R7
	STR R1,R6,#0
	STR R2,R6,#1
	STR R3,R6,#2
	STR R5,R6,#3
	STR R7,R6,#4
	AND R1,R1,#0		; clear the current value
	LD R2,RN_NEG_0		; put the value -x30 in R2
	AND R5,R5,#0		; no digits yet
	ST R5,RN_NEGATE
READ_LOOP
	JSR BUF_GETC
	ADD R0,R4,R2		; subtract x30 from R4 and store in R0 
	BRn RN_NON_DIG		; smaller than '0' means a non-digit
	ADD R3,R0,#-10		; check if > '9'
	BRzp RN_NON_DIG		; greater than '9' means a non-digit
	ADD R5,R4,#0		; a digit has been seen
	ADD R3,R1,R1		; sequence of adds multiplies R1 by 10
	ADD R3,R3,R3
	ADD R1,R1,R3
	ADD R1,R1,R1
	ADD R1,R1,R0		; finally, add in new digit
	BRnzp READ_LOOP		; get another digit
RN_NON_DIG
	; if we see space, tab, CR, or LF, we consume if no digits have
	; been seen; otherwise, we stop and buffer the character
	AND R0,R0,#0
	ADD R5,R5,#0
	BRp RN_GOT_NUM 
	BRz RN_NO_DIGITS

	; need to put the minus sign back, too
	JSR BUF_UNGETC
	LD R4,RN_MINUS
	BRnzp RN_SAVE_CHAR

RN_NO_DIGITS
	ADD R3,R4,#-9
	BRz READ_LOOP
	ADD R3,R4,#-10
	BRz READ_LOOP
	ADD R3,R4,#-13
	BRz READ_LOOP
	ADD R3,R4,#-16
	ADD R3,R3,#-16
	BRz READ_LOOP

	LD R3,RN_NEG_MIN
	ADD R3,R3,R4
	BRnp RN_SAVE_CHAR
	ADD R5,R5,#-1
	ST R5,RN_NEGATE
	BRnzp READ_LOOP

RN_GOT_NUM
	ADD R0,R0,#1
	LD R5,RN_NEGATE
	BRz RN_SAVE_CHAR
	NOT R1,R1
	ADD R1,R1,#1
RN_SAVE_CHAR
	JSR BUF_UNGETC
	ADD R4,R1,#0		; move R1 into R4	
	LDR R1,R6,#0		; restore R1, R2, R3, R5, and R7
	LDR R2,R6,#1
	LDR R3,R6,#2
	LDR R5,R6,#3
	LDR R7,R6,#4
	ADD R6,R6,#5
	RET

RN_NEG_0	.FILL xFFD0	; -'0'
RN_NEG_MIN	.FILL xFFD3	; -'-'
RN_MINUS	.FILL x002D	; '-'
RN_NEGATE	.BLKW 1

; SCANF -- scan in formatted data
;     INPUTS -- format string followed by arguments
;     OUTPUTS -- number of integers converted left on top of stack 
;                (return value)
;     SIDE EFFECTS -- uses stack to save registers
;     NOTE: call as a C function
;
; R0 holds the character to be read
; R1 is the format string pointer
; R2 points to the next argument
; R3 is the number of integer conversions so far
; R4 is the character/number actually read from the keyboard
;
SCANF	ADD R6,R6,#-6		; save R0, R1, R2, R3, R4, and R7
	STR R0,R6,#0
	STR R1,R6,#1
	STR R2,R6,#2
	STR R3,R6,#3
	STR R4,R6,#4
	STR R7,R6,#5
	LDR R1,R6,#6
	ADD R2,R6,#7
	AND R3,R3,#0
SC_LOOP	JSR LOAD_FORMAT
	ADD R0,R0,#0
	BRz SC_DONE
	BRp SC_REG
	JSR READ_NUM
	ADD R0,R0,#0
	BRz SC_DONE
	LDR R0,R2,#0
	ADD R2,R2,#1
	STR R4,R0,#0
	ADD R3,R3,#1
	BRnzp SC_LOOP
SC_REG	JSR BUF_GETC
	NOT R0,R0
	ADD R0,R0,#1
	ADD R0,R0,R4
	BRz SC_LOOP
	JSR BUF_UNGETC
SC_DONE	LDR R7,R6,#5		; restore R7
	STR R3,R6,#5		; save return value
	LDR R0,R6,#0		; restore R0, R1, R2, R3, and R4
	LDR R1,R6,#1
	LDR R2,R6,#2
	LDR R3,R6,#3
	LDR R4,R6,#4
	ADD R6,R6,#5
	RET

; buffered input characters (0 means none)
INBUF	.FILL x0000
INBUF2	.FILL x0000

;---------------------------------------------------------------------------
; global data space allocation
;---------------------------------------------------------------------------

GLOBDATA
