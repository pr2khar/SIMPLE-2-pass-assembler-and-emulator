; test04
; Memory store test

; program stores larger of the two numbers to memory location 00000000


main:   ldc onee ; A has 1
	ldc twoo ; A has -2 and B has 1
	sub ; A = B-A .. A has 3 and B has 1
	brlz else
if:	ldc onee
	br 1
else:	ldc twoo
	stl 0
	HALT
onee: SET -2
twoo: SET 2