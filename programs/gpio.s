ldr r0,=0x20200000

set_output:
mov r1,#1
lsl r1,#18
str r1,[r0,#0x4]

loop:

led16_off:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x1C]


ldr r2,#0xFFFFFF
wait_a:
sub r2,r2,#1
cmp r2,#0
bne wait_a

led16_on:
mov r1,#1
lsl r1,#16
str r1,[r0,#0x28]

ldr r2,#0xFFFFFF
wait_b:
sub r2,r2,#1
cmp r2,#0
bne wait_b

b loop
