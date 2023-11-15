; file ps.as with errors
; error in line 16 - value 150000 is out of boundries
; error in linke 10 r9 is not a valid register
.entry LENGTH
.extern W
MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
prn -5
bne W
sub @r1, @r9
bne L3
L1: inc K
.entry LOOP
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,150000
K: .data 22
.extern L3
