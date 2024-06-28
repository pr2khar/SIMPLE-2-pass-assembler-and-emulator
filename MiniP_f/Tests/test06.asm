; test06
; addition of two numbers


ldc a ; loads address a
ldnl 0 ; loads value from a into A
ldc b ; loads address b
ldnl 0 ; loads value from b into A. Prior A value goes into B.
add ; A+B is loaded to A
stl 0 ; A is stored in memory location 0
HALT


a: data 36
b: data 64