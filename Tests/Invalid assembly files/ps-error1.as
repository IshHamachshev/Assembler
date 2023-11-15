; file ps.as with errors
; unknown command in line 8
; unknown entry in line 13 (LOOK)
.entry LENGTH
.extern W
MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
prqn -5
bne W
sub @r1, @r4
bne L3
L1: inc K
.entry LOOK
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
.extern L3
