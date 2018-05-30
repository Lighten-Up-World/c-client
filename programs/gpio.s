ldr r0,=0x20200000

mov r1,#1
lsl r1,#4
str r1,[r0,#28]

mov r1,#0xFFF3FFFF
str r1,[r0,#4]

mov r1,#1
lsl r1,#4
str r1,[r0,#20]


cmp r0,#0

mov r2,#10
wait2:
sub r2,r2,#1
cmp r2,#0
bne wait2
