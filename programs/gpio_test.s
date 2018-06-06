ldr r0,=0x20200000

mov r1,#1
lsl r1,#18
str r1,[r0,#4]

mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]

str r1,[r0,#0x1C]

mov r1,#1
lsl r1,#20
wait:
sub r1,r1,#1
cmp r1,#1
bne wait

andeq r0,r0,r0
