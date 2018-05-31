ldr r0,=0x20200000
mov r1,#1
lsl r1,#18

str r1,[r0,#4]

mov r1,#1
lsl r1,#16

str r1,[r0,#36]

sub r2,r2,#1

str r1,[r0,#28]

cmp r0,#0

on:
bne on

