ldr r0,=0x20200000

mov r1,#1
lsl r1,#22
str r1,[r0,#40]

turn_off:
mov r1,#1
lsl r1,#6
str r1,[r0,#8]

set_off_ret_addr:
mov r2,#1
b wait

turn_on:
mov r1,#1
lsl r1,#22
str r1,[r0,#28]

set_on_ret_addr:
mov r2,#0
b exit

wait:
mov r1,#1
lsl r1,#15
sub:
sub r1,r1,#1
cmp r1,#1
bne sub
cmp r2,#0
bne turn_on
b turn_off

exit:
andeq r0,r0,r0
