ldr r0,=0x20200000

output:
mov r1,#1
lsl r1,#18
str r1,[r0,#4]

on:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]
mov r0, #1
b wait

off:
str r1,[r0,#0x1C]
mov r0, #0
b wait

wait:
mov r2,#1
lsl r2,#8
loop:
sub r2,r2,#1
cmp r2,#0
bne loop
cmp r0, #0
be on
b off

andeq r0,r0,r0
