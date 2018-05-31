mov r2,#1
lsl r2,#18
ldr r0,=0x20200000
str r2,[r0,#4]

mov r1,#1
lsl r1,#16
str r1,[r0,#40] #40 is clear

str r1,[r0,#28] #28 is on

loop:
lsl r1,#1

b loop