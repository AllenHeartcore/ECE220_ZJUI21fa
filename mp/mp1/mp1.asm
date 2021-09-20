; This is the program for ECE220FA21 @ ZJUI Institute, Machine Programming Problem I. 
; Written, commented and debugged by Chen, Ziyuan on 14 September 2021. 
; Refined by Chen, Ziyuan on 16 September 2021. 
; Fixed a small bug, and commited on 20 September 2021. 
; Copyright © 2021 Chen, Ziyuan

               .ORIG      x3000

                LEA       R0, TestString
                JSR       STRLEN
                JSR       PRINT_DECIMAL
                HALT

SaveRs          ST        R0, SaveR0
                ST        R1, SaveR1
                ST        R2, SaveR2
                ST        R3, SaveR3
                ST        R4, SaveR4
                ST        R5, SaveR5
                ST        R6, SaveR6
                RET

LoadRs          LD        R0, SaveR0
                LD        R1, SaveR1
                LD        R2, SaveR2
                LD        R3, SaveR3
                LD        R4, SaveR4
                LD        R5, SaveR5
                LD        R6, SaveR6
                RET
                
STRLEN          AND       R1, R1, #0
                ST        R0, SaveR0
                ST        R6, SaveR6
CharExamine     LDR       R6, R0, #0
                BRz       CharExamined
                ADD       R1, R1, #1
                ADD       R0, R0, #1
                BRnzp     CharExamine
CharExamined    LD        R0, SaveR0
                LD        R6, SaveR6
                RET

; ######   Register Table for the PRINT_DECIMAL Subroutine   ######
; Register               |    Function        |   Description
; -----------------------+--------------------+-----------------------------------------------
; R0 - Pointer 3         |   *PrintHeader     |   Filters out leading zeros. 
; R1 - Operand (Given)   |   <External>       |   Number to be printed. 
; R2 - Pointer 1         |   *TenPowers       |   R6 <- M[R2]
; R3 - Pointer 2         |   *PrintBuffer     |   M[R3] <- R5
; R4 - Counter 1         |    DigitPosition   |   0 <= R4 <= 5
; R5 - Counter 2         |    DigitValue      |   0 <= R4 <= 9
; R6 - Operand (Temp.)   |   <Dynamic>        |   TenPowers or ASCIIZero. See comments below. 
; R7 - Control Flow Maintainer                |   [R7 Enclosing] applied at JSRs and TRAPs. 

PRINT_DECIMAL   ADD       R1, R1, #0
                BRz       PrintZero
                ST        R7, SaveR7
                JSR       SaveRs             ; [R7 Enclosing] Save Registers 0~6
                LD        R7, SaveR7
                LEA       R2, TenPowers      ; <R2 init. >
                LEA       R3, PrintBuffer    ; <R3 init. >
                LEA       R0, PrintBuffer    ; <R0 init. >
                AND       R4, R4, #0
                ADD       R4, R4, #5         ; <R4 init. >
DigitExamine    AND       R5, R5, #0         ; <R5 init. >
                LDR       R6, R2, #0         ; <R6 init. I  > (TenPowers)
                NOT       R6, R6
                ADD       R6, R6, #1         ; <R6 init. II > (-> -TenPowers)
DigitCount      ADD       R1, R1, R6
                BRn       DigitExamined      ; Count and ...
                ADD       R5, R5, #1         ; ... accumulate DigitValue
                BRnzp     DigitCount
DigitExamined   NOT       R6, R6
                ADD       R6, R6, #1         ; <R6 init. III> (-> TenPowers)
                ADD       R1, R1, R6         ; Recover R1 by one TenPowers
                LD        R6, ASCIIZero      ; <R6 init. IV > (-> ASCIIZero)
                ADD       R6, R5, R6   
                STR       R6, R3, #0         ; Calculates and stores characterized DigitValue
                ADD       R3, R3, #1         ; Update *PrintBuffer
                ADD       R2, R2, #1         ; Update *TenPowers
                ADD       R4, R4, #-1        ; Update DigitPosition
                BRz       PrintNumber        ; Ready for printing when DigitPosition == 0
                BRp       DigitExamine       ; Update DigitValue and TenPowers
PrintZero       LD        R0, ASCIIZero
                ST        R7, SaveR7
                OUT                          ; [R7 Enclosing] A pre-determined special case
                LD        R7, SaveR7
                BRnzp     PrintDone
PrintNumber     LD        R6, ASCIIZeroNeg   ; <R6 init. V  > (-> ASCIIZeroNeg)
PrintHeader     LDR       R5, R0, #0
                ADD       R5, R5, R6
                BRp       PrintNumberGo
                ADD       R0, R0, #1         ; Locate *PrintHeader
                BRnzp     PrintHeader
PrintNumberGo   ST        R7, SaveR7
                PUTS                         ; [R7 Enclosing] R0 = *PrintHeader
                LD        R7, SaveR7
PrintDone       ST        R7, SaveR7
                JSR       LoadRs             ; [R7 Enclosing] Recover Registers 0~6
                LD        R7, SaveR7
                RET

TenPowers      .FILL      10000
               .FILL      1000
               .FILL      100
               .FILL      10
               .FILL      1
ASCIIZero      .FILL      x0030
ASCIIZeroNeg   .FILL      xFFD0
PrintBuffer    .BLKW      6
SaveR0         .FILL      x0000
SaveR1         .FILL      x0000
SaveR2         .FILL      x0000
SaveR3         .FILL      x0000
SaveR4         .FILL      x0000
SaveR5         .FILL      x0000
SaveR6         .FILL      x0000
SaveR7         .FILL      x0000
TestString     .STRINGZ   "This is a test string. "

               .END
