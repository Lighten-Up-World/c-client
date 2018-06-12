ldr r0,=0x20200000

output:
mov r1,#1
lsl r1,#18
str r1,[r0,#4]

on:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]

andeq r0,r0,r0
