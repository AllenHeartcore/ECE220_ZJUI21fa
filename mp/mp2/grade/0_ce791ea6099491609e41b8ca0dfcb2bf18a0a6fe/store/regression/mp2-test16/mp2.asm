; ###### Copyright Information ######
; This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem II. 
; Written, commented and debugged by Chen, Ziyuan on 26~27 September 2021. 
; Refined and committed by Chen, Ziyuan on 28 September 2021. 
; Copyright © 2021 Chen, Ziyuan. All rights reserved. 

; ######  External References  ######
; Chen, Ziyuan. "SaveRs," "LoadRs," "STRLEN," "PRINT_DECIMAL" subroutines, 
; 	and corresponding look-up tables. Machine Programming Problem I. 
; 	20 September 2021. Line 14~42, 56~120. 


		.ORIG	x3000


		JSR	FIND_M_N		; <Find M, N>
		LD	R0, AddrString1		; <Print Line 1>
		PUTS				; 	String 1
		LEA	R0, MsgLine1
		PUTS				; 	" -> "
		LD	R0, AddrString2
		PUTS				; 	String 2
		LEA	R0, MsgLine2		; <Print Line 2>
		PUTS				; 	"\nL. d. = "
		LDI	R1, AddrLevDist
		JSR	PRINT_DECIMAL		; 	L. d. 
		LD	R0, MsgLinefeed
		OUT				; 	"\n"
		JSR	SaveRs			; <Print Lines 3&4>
		JSR	PRETTY_PRINT
		JSR	LoadRs
		HALT


FIND_M_N	ST	R7, SaveR7
		JSR	SaveRs			; Callee-save R0~R6
		LD	R0, AddrString1		; {
		JSR	STRLEN			; 	Find M = len(Str1) + 1
		LD	R0, AddrM
		ADD	R1, R1, #1
		STR	R1, R0, #0		; }
		LD	R0, AddrString2		; {
		JSR	STRLEN			; 	Find N = len(Str2) + 1
		LD	R0, AddrN
		ADD	R1, R1, #1
		STR	R1, R0, #0		; }
		JSR	LoadRs			; Reload R0~R6
		LD	R7, SaveR7
		RET


; ###### Register Table for PRETTY_PRINT ######
; 	<Phase>		Init		Explore		Print
; 	R0		M * N				CharForPrint
; 	R1		M		StackCounter	StackCounter
; 	R2		N				StackCounterBackup
; 	R3				TableDataOffset	StringPointer
; 	R4				TableDataType	StackData
; 	R5				TableExplorer	StackPointerBackup
; 	R6				StackPointer	StackPointer
; 	R7		< Reserved as Control Flow Maintainer >

PRETTY_PRINT	ST	R7, SaveR7
		AND	R0, R0, #0		; <Phase 1: Init>
		LDI	R1, AddrM		; 	R1 = M
		LDI	R2, AddrN		; 	R2 = N
		LD	R5, AddrTable		; 	R5 = x4000
		LD	R6, StackBase		; 	R6 = x8000
MultiplyMN	ADD	R0, R0, R1
		ADD	R2, R2, #-1
		BRp	MultiplyMN		; 	R1 = MN when loop breaks
		ADD	R1, R0, R0		; 	R1 = 2MN
		ADD	R0, R0, R1		; 	R0 = 3MN
		ADD	R5, R5, R0		; 	R5 = x4000 + 3MN
		ADD	R5, R5, #-2		; 	R5 = Last address of Offset
		AND	R1, R1, #0		; <Phase 2: Explore>
Explore		LDR	R3, R5, #0		; 	R3 = Offset
		BRz	PrintString
		LDR	R4, R5, #1		; 	R4 = Type
		ADD	R6, R6, #-1
		STR	R4, R6, #0		; 	Push R4 into Stack
		ADD	R1, R1, #1		; 	Increment StackCounter
		ADD	R5, R5, R3		; 	Explore forward by one step
		BRnzp	Explore
PrintString	ADD	R2, R1, #0		; <Phase 3: Print>
		ADD	R6, R6, #-1		; 	Adjust StackPointer to just over the top
		ADD	R5, R6, #0		; 	Backup StackCounter and StackPointer
		LD	R3, AddrString1		;   - Pretty print String1
ExamineStack1	ADD	R1, R1, #-1		; 	Decrement StackCounter1
		BRn	ExamineNext1
		ADD	R6, R6, #1
		LDR	R4, R6, #0		; 	Load StackData
		BRz	PrintEmpty1
		LDR	R0, R3, #0		; 	  - The D/MS cases
		OUT
		ADD	R3, R3, #1
		BRnzp	ExamineStack1
PrintEmpty1	LD	R0, MsgEmpty		; 	  - The I case
		OUT
		BRnzp	ExamineStack1
ExamineNext1	LD	R0, MsgLinefeed		;   - Task interval 1
		OUT				; 	Print a linefeed
		ADD	R1, R2, #0		; 	Recover StackCounter
		ADD	R6, R5, #0		; 	Recover StackPointer
		LD	R3, AddrString2		;   - Pretty print String2
ExamineStack2	ADD	R1, R1, #-1		; 	Decrement StackCounter1
		BRn	ExamineNext2
		ADD	R6, R6, #1
		LDR	R4, R6, #0		; 	Load StackData
		ADD	R4, R4, #-1
		BRz	PrintEmpty2
		LDR	R0, R3, #0		; 	  - The I/MS cases
		OUT
		ADD	R3, R3, #1
		BRnzp	ExamineStack2
PrintEmpty2	LD	R0, MsgEmpty		; 	  - The D case
		OUT
		BRnzp	ExamineStack2
ExamineNext2	LD	R0, MsgLinefeed		;   - Task interval 2
		OUT				; 	Print a linefeed
		LD	R7, SaveR7
		RET


SaveRs		ST	R0, SaveR0
		ST	R1, SaveR1
		ST	R2, SaveR2
		ST	R3, SaveR3
		ST	R4, SaveR4
		ST	R5, SaveR5
		ST	R6, SaveR6
		RET


LoadRs		LD	R0, SaveR0
		LD	R1, SaveR1
		LD	R2, SaveR2
		LD	R3, SaveR3
		LD	R4, SaveR4
		LD	R5, SaveR5
		LD	R6, SaveR6
		RET


STRLEN		ST	R0, SaveR0_STRLEN
		ST	R6, SaveR6_STRLEN
		AND	R1, R1, #0
CharExamine	LDR	R6, R0, #0
		BRz	CharExamined
		ADD	R1, R1, #1
		ADD	R0, R0, #1
		BRnzp	CharExamine
CharExamined	LD	R0, SaveR0_STRLEN
		LD	R6, SaveR6_STRLEN
		RET


PRINT_DECIMAL	ST	R7, SaveR7
		ADD	R1, R1, #0
		BRz	PrintZero
		JSR	SaveRs
		LEA	R2, TenPowers
		LEA	R3, PrintBuffer
		LEA	R0, PrintBuffer
		AND	R4, R4, #0
		ADD	R4, R4, #5
DigitExamine	AND	R5, R5, #0
		LDR	R6, R2, #0
		NOT	R6, R6
		ADD	R6, R6, #1
DigitCount	ADD	R1, R1, R6
		BRn	DigitExamined
		ADD	R5, R5, #1
		BRnzp	DigitCount
DigitExamined	NOT	R6, R6
		ADD	R6, R6, #1
		ADD	R1, R1, R6
		LD	R6, ASCIIZero
		ADD	R6, R5, R6
		STR	R6, R3, #0
		ADD	R3, R3, #1
		ADD	R2, R2, #1
		ADD	R4, R4, #-1
		BRz	PrintNumber
		BRp	DigitExamine
PrintZero	LD	R0, ASCIIZero
		OUT
		BRnzp	PrintDone
PrintNumber	LD	R6, ASCIIZeroNeg
PrintHeader	LDR	R5, R0, #0
		ADD	R5, R5, R6
		BRp	PrintNumberDo
		ADD	R0, R0, #1
		BRnzp	PrintHeader
PrintNumberDo	PUTS
		JSR	LoadRs
PrintDone	LD	R7, SaveR7
		RET


AddrString1	.FILL	x3800
AddrString2	.FILL	x3840
AddrLevDist	.FILL	x38C0
AddrM		.FILL	x38E0
AddrN		.FILL	x38E1
AddrTable	.FILL	x4000
StackBase	.FILL	x8000
MsgLine1	.STRINGZ " -> "
MsgLine2	.STRINGZ "\nLevenshtein distance = "
MsgLinefeed	.STRINGZ "\n"
MsgEmpty	.STRINGZ "-"
TenPowers	.FILL	#10000
		.FILL	#1000
		.FILL	#100
		.FILL	#10
		.FILL	#1
ASCIIZero	.FILL	x0030
ASCIIZeroNeg	.FILL	xFFD0
PrintBuffer	.BLKW	#6
SaveR0_STRLEN	.FILL	x0000
SaveR6_STRLEN	.FILL	x0000
SaveR0		.FILL	x0000
SaveR1		.FILL	x0000
SaveR2		.FILL	x0000
SaveR3		.FILL	x0000
SaveR4		.FILL	x0000
SaveR5		.FILL	x0000
SaveR6		.FILL	x0000
SaveR7		.FILL	x0000


		.END
