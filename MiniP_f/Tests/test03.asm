; test03.asm
; Branch test


ldc var2
; A = 6
ldc var1
; A = 100 and B = 6
brlz 1
; if A<0, branch (will not branch)
adc -120
; A = -20
add
; A = -14
brlz -2
; if A,0, branch to add. this will happen 3 times when A = -14, then -8, then -2
; when A becomes 4, we continue
HALT

var1: SET 100
var2: SET 6