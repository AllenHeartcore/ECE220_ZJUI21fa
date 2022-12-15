        .ORIG x3000

        JSR FIND_M_N
        JSR INIT_WORK
        JSR CALC_DISTANCE
        LEA R0,TTLD_MSG
        PUTS
        JSR PRINT_DECIMAL
        HALT

TTLD_MSG  .STRINGZ "Levenshtein distance = "
; ###### Copyright Information ######
; This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem III. 
; Written and commented by Chen, Ziyuan on 29 September 2021. 
; Debugged and committed by Chen, Ziyuan on 30 September 2021. 
; Refined and re-committed by Chen, Ziyuan on 13 October 2021. 
; Copyright © 2021 Chen, Ziyuan. All rights reserved. 

; ######  External References  ######
; Chen, Ziyuan. "SaveRs," "LoadRs," "STRLEN," "PRINT_DECIMAL," "FIND_M_N," "PRETTY_PRINT" 
; 	subroutines, and corresponding lookup tables. Machine Programming Problem II. 
; 	28 September 2021. Line 35~49, 63~229. 




		JSR	FIND_M_N
		LD	R0, AddrString1
		PUTS
		LEA	R0, MsgLine1
		PUTS
		LD	R0, AddrString2
		PUTS
		LEA	R0, MsgLine2
		PUTS
		JSR	INIT_WORK
		JSR	CALC_DISTANCE
		JSR	PRINT_DECIMAL
		LD	R0, MsgLinefeed
		OUT
		JSR	PRETTY_PRINT
		HALT


; ###### Register Table for INIT_WORK ######
; 	Register	Phase1Init	Phase2Init	Phase3Init	Description
; 	R0 - CurrCost 	<Unused>	CostI		CostD		Field 1 adjuster
; 	R1 - CurrLevD 	0		0<Init.ed>	0		Field 1
; 	R2 - CurrOfst 	0		-3M		-3		Field 2
; 	R3 - CurrType 	-1		0		1		Field 3
; 	R4 - TableCtr 	<Unused>	N-1		M-1		Table counter
; 	R5 - TablePtr 	x4000		x4000<Init.ed>	x4000		Table pointer
; 	R6 - TableAdj 	<Unused>	3M		3		Table pointer adjuster

INIT_WORK				; < Phase 1: Init first cell >
		AND	R1, R1, #0	; 	CurrLevD = 0
		AND	R2, R2, #0	; 	CurrOfst = 0
		AND	R3, R3, #0
		ADD	R3, R3, #-1	; 	CurrType = -1
		LD	R5, AddrTable	; 	TablePtr = x4000
		STR	R1, R5, #0
		STR	R2, R5, #1
		STR	R3, R5, #2	; 	Fill the table
					; < Phase 2: Init first column >
		LDI	R0, AddrCostI	; 	CurrCost = CostI
		AND	R3, R3, #0	; 	CurrType = 0
		LDI	R4, AddrM
		ADD	R6, R4, R4
		ADD	R6, R6, R4	; 	TableAdj = 3M
		NOT	R2, R6
		ADD	R2, R2, #1	; 	CurrOfst = -3M
		ST	R2, OffsetI	; 	! Save for CALC_DISTANCE !
		ADD	R4, R2, #-3
		ST	R4, OffsetS	; 	! Save for CALC_DISTANCE !
		LDI	R4, AddrN
InitColumn	ADD	R4, R4, #-1	; 	Decrement counter
		BRz	InitNext
		ADD	R1, R1, R0	; 	CurrLevD += CurrCost
		ADD	R5, R5, R6	; 	TablePtr += TableAdj
		STR	R1, R5, #0
		STR	R2, R5, #1
		STR	R3, R5, #2	; 	Fill the table
		BR	InitColumn
					; < Phase 3: Init first row >
InitNext	LDI	R0, AddrCostD	; 	CurrCost = CostD
		AND	R1, R1, #0	; 	CurrLevD = 0
		LD	R2, OffsetD	; 	CurrOfst = -3
		ADD	R3, R3, #1	; 	CurrType = 1
		LDI	R4, AddrM
		LD	R5, AddrTable	; 	TablePtr = x4000
		AND	R6, R6, #0
		ADD	R6, R6, #3	; 	TableAdj = 3
InitRow		ADD	R4, R4, #-1	; 	Decrement counter
		BRz	InitDone
		ADD	R1, R1, R0	; 	CurrLevD += CurrCost
		ADD	R5, R5, R6	; 	TablePtr += TableAdj
		STR	R1, R5, #0
		STR	R2, R5, #1
		STR	R3, R5, #2	; 	Fill the table
		BR	InitRow
InitDone	RET


; ###### Register Table for CALC_DISTANCE ######
; 	Register			Description
; 	R0 - BestOfst/BestType		(Field 2, 3) Table loader
; 	R1 - BestLevD			(Field 1) Best L.D. ever seen
; 	R2 - StrInsp1/CandLevDNeg	String pointer & char. 1 / Negated cand. L.D.
; 	R3 - StrInsp2/CostOp		String pointer & char. 2 / Cost of operations
; 	R4 - TableInsp/CandLevD		Pointer to inspecting cell / Candidate L.D.
; 	R5 - TableFill			Pointer to current unfilled cell
; 	R6 - CounterC			Downcounts C
; 	R7 - CounterR			Downcounts R

CALC_DISTANCE	ST	R7, SaveR7
					; < Phase 1: Init registers >
		LD	R5, AddrTable
		LDI	R6, AddrM
		ADD	R7, R6, R6
		ADD	R5, R5, R6
		ADD	R5, R5, R7
		ADD	R5, R5, #3	; 	TableFill = x4000+3M+3
		LDI	R7, AddrN
		ADD	R6, R6, #-1	; 	CounterC = M-1
		BRz	InspectDone
		ADD	R7, R7, #-1	; 	CounterR = N-1
		BRz	InspectDone
					; < Phase 2: Inspect cell >
InspectCell	LD	R4, OffsetS	;   - M/S Type
		ADD	R4, R5, R4	; 	Set TableInsp
		LDR	R4, R4, #0	; 	Set CandLevD
		ADD	R1, R4, #0	; 	Init. BestLevD
		LD	R2, StrInsp1
		LDR	R3, R2, #0	; 	Character from String1
		ADD	R2, R2, #1
		ST	R2, StrInsp1	; 	Increment StrInsp1
		LD	R2, StrInsp2
		LDR	R2, R2, #0	; 	Character from String2
		NOT	R2, R2
		ADD	R2, R2, #1
		ADD	R3, R2, R3	; 	Compare the characters
		BRz	InspectMatch
		LDI	R3, AddrCostS
		ADD	R1, R1, R3	; 	[F] BestLevD += CostS if mismatch
InspectMatch	LD	R0, OffsetS
		STR	R0, R5, #1	; 	Fill OffsetS into table
		AND	R0, R0, #0
		ADD	R0, R0, #2
		STR	R0, R5, #2	; 	Fill TypeS(2) into table
		LD	R4, OffsetD	;   - D Type
		ADD	R4, R5, R4	; 	Set TableInsp
		LDR	R4, R4, #0	; 	Set CandLevD
		LDI	R3, AddrCostD
		ADD	R4, R4, R3	; 	CandLevD += CostD
		NOT	R2, R4
		ADD	R2, R2, #1
		ADD	R2, R1, R2	; 	Compare CandLevD and BestLevD
		BRnz	InspectInsert
		ADD	R1, R4, #0	; 	[T] Accept CandLevD (< BestLevD)
		LD	R0, OffsetD
		STR	R0, R5, #1	; 	[T] Fill OffsetD into table
		AND	R0, R0, #0
		ADD	R0, R0, #1
		STR	R0, R5, #2	; 	[T] Fill TypeD(1) into table
InspectInsert	LD	R4, OffsetI	;   - I Type
		ADD	R4, R5, R4	; 	Set TableInsp
		LDR	R4, R4, #0	; 	Set CandLevD
		LDI	R3, AddrCostI
		ADD	R4, R4, R3	; 	CandLevD += CostI
		NOT	R2, R4
		ADD	R2, R2, #1
		ADD	R2, R1, R2	; 	Compare CandLevD and BestLevD
		BRnz	InspectNxtCell
		ADD	R1, R4, #0	; 	[T] Accept CandLevD (< BestLevD)
		LD	R0, OffsetI
		STR	R0, R5, #1	; 	[T] Fill OffsetI into table
		AND	R0, R0, #0
		STR	R0, R5, #2	; 	[T] Fill TypeI(0) into table
InspectNxtCell	STR	R1, R5, #0	; 	Fill BestLevD into table
					; < Phase 3: Jump to next cell >
		ADD	R5, R5, #3	; 	TableFill += 3
		ADD	R6, R6, #-1	; 	Decrement CounterC
		BRz	InspectNxtRow
		BRp	InspectCell
InspectNxtRow	LD	R2, AddrString1
		ST	R2, StrInsp1	; 	[T] Reset StrInsp1
		LD	R3, StrInsp2
		ADD	R3, R3, #1	; 	[T] Increment StrInsp2
		ST	R3, StrInsp2
		ADD	R5, R5, #3	; 	[T] Another TableFill += 3
		LDI	R6, AddrM
		ADD	R6, R6, #-1	; 	[T] Reset CounterC = M-1
		ADD	R7, R7, #-1	; 	[T] Decrement CounterR
		BRz	InspectDone
		BRp	InspectCell
InspectDone	LD	R7, SaveR7	; < Done > R1 stores the latest BestLevD
		RET


AddrString1	.FILL	x3800
AddrString2	.FILL	x3840
AddrCostI	.FILL	x3880
AddrCostD	.FILL	x3881
AddrCostS	.FILL	x3882
AddrM		.FILL	x38E0
AddrN		.FILL	x38E1
OffsetI		.FILL	x0000
OffsetD		.FILL	xFFFD
OffsetS		.FILL	x0000
StrInsp1	.FILL	x3800
StrInsp2	.FILL	x3840
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


FIND_M_N	ST	R7, SaveR7
		JSR	SaveRs
		LD	R0, AddrString1
		JSR	STRLEN
		LD	R0, AddrM
		ADD	R1, R1, #1
		STR	R1, R0, #0
		LD	R0, AddrString2
		JSR	STRLEN
		LD	R0, AddrN
		ADD	R1, R1, #1
		STR	R1, R0, #0
		JSR	LoadRs
		LD	R7, SaveR7
		RET


PRETTY_PRINT	ST	R7, SaveR7
		AND	R0, R0, #0
		LDI	R1, AddrM
		LDI	R2, AddrN
		LD	R5, AddrTable
		LD	R6, StackBase
MultiplyMN	ADD	R0, R0, R1
		ADD	R2, R2, #-1
		BRp	MultiplyMN
		ADD	R1, R0, R0
		ADD	R0, R0, R1
		ADD	R5, R5, R0
		ADD	R5, R5, #-2
		AND	R1, R1, #0
Explore		LDR	R3, R5, #0
		BRz	PrintString
		LDR	R4, R5, #1
		ADD	R6, R6, #-1
		STR	R4, R6, #0
		ADD	R1, R1, #1
		ADD	R5, R5, R3
		BRnzp	Explore
PrintString	ADD	R2, R1, #0
		ADD	R6, R6, #-1
		ADD	R5, R6, #0
		LD	R3, AddrString1
ExamineStack1	ADD	R1, R1, #-1
		BRn	ExamineNext1
		ADD	R6, R6, #1
		LDR	R4, R6, #0
		BRz	PrintEmpty1
		LDR	R0, R3, #0
		OUT
		ADD	R3, R3, #1
		BRnzp	ExamineStack1
PrintEmpty1	LD	R0, MsgEmpty
		OUT
		BRnzp	ExamineStack1
ExamineNext1	LD	R0, MsgLinefeed
		OUT
		ADD	R1, R2, #0
		ADD	R6, R5, #0
		LD	R3, AddrString2
ExamineStack2	ADD	R1, R1, #-1
		BRn	ExamineNext2
		ADD	R6, R6, #1
		LDR	R4, R6, #0
		ADD	R4, R4, #-1
		BRz	PrintEmpty2
		LDR	R0, R3, #0
		OUT
		ADD	R3, R3, #1
		BRnzp	ExamineStack2
PrintEmpty2	LD	R0, MsgEmpty
		OUT
		BRnzp	ExamineStack2
ExamineNext2	LD	R0, MsgLinefeed
		OUT
		LD	R7, SaveR7
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



.END
