.entry LIST
.extern W
MAIN: add @r3, LIST
LOOP: prn 48
mcro m1
 inc @r6
 mov @r3, W
endmcro
mcro m2
;m2 macro first line comment
;   m2 macro second line comment
endmcro
mcro m3
;m3 macro first line comment
;   m3 macro second line comment
;   m3 macro third line comment
endmcro
STR: .string "abcd"
LIST: .data 6, -9
lea STR, @r6
inc @r6
mov @r3, W
sub @r1, @r4
m2
bne END
cmp val1, -6
m3
bne END
dec K
.entry MAIN
sub LOOP , @r5
END: stop
.data -100
.entry K
m1
K: .data 31
.extern val1
m1
