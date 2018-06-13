ldr r0,=0x20200000

set_output:
mov r1,#1
lsl r1,#18
str r1,[r0,#0x4]



led16_off:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x1C]



mov r2,#1
lsl r2,#6
wait_a:
sub r2,r2,#1
cmp r2,#0
bne wait_a


led16_on:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]


mov r2,#1
lsl r2,#6
wait_b:
sub r2,r2,#1
cmp r2,#0
bne wait_b


led16_off_b:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x1C]


mov r2,#1
lsl r2,#6
wait_c:
sub r2,r2,#1
cmp r2,#0
bne wait_c


led16_on_b:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]


andeq r0,r0,r0

