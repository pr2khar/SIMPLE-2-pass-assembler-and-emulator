; test05
; while loop implementation
; while(i<0) i++
; initially i = -500
ldc -500
loop:
adc 1
brlz loop
HALT