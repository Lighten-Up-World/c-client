ldr r0,=0x20200000

set_output:
mov r1,#1
lsl r1,#18
str r1,[r0,#0x4]

led16_on:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]

andeq r0,r0,r0
