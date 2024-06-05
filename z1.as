; file z1.as
.entry LIST
.extern W
mcr mymcr
	cmp K , #sz
	bne W
endmcr
.define sz = 2
MAIN:	mov r3 , LIST[sz]
LOOP: 	jmp W
	prn #-5
	mov STR[5] , STR[2]
	sub r1 , r4
mymcr
L1: 	inc L3
.entry LOOP
	bne LOOP
END: hlt
.define len=4
STR: .string "abcdef"
HELLO: hlt
LIST: .data 6 , -9 , len
X:  .data 10,8,2
K: .data 22
.extern L3
