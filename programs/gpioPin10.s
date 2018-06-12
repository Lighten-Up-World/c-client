#NOT WORKING

ldr r0,=0x20200000

mov r1,#1
lsl r1,#10
str r1,[r0,#36] #+0x24 is clear

mov r2,#1

mov r3,#1
lsl r3,#4
add r2,r3,r2

mov r3,#1
lsl r3,#7
add r2,r3,r2

mov r3,#1
lsl r3,#10
add r2,r3,r2

mov r3,#1
lsl r3,#13
add r2,r3,r2

mov r3,#1
lsl r3,#16
add r2,r3,r2

mov r3,#1
lsl r3,#19
add r2,r3,r2

mov r3,#1
lsl r3,#22
add r2,r3,r2

mov r3,#1
lsl r3,#25
add r2,r3,r2

mov r3,#1
lsl r3,#28
add r2,r3,r2

str r2,[r0,#4] #set as output

mov r1,#1
str r1,[r0,#28] #+0x1C is on

loop:
lsl r1,#1

b loop

