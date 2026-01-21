.def A r0
.def CNT 10

begin:
entry:
clr  r0
inc  r0
incc r1
add  r0, r1
addc r1, r2
and  r2, r3
or   r3, r4
xor  r4, r5
mul  r5, r6
mul  r6, CNT
inv  r7
swap r0
lsr  r1
lsrc r2
mov  r1, r0
lpm  r2, XI
lpmw r2, XI  ; r3:r2 := *X++
ld   r3, Y
ld   r3, YD - 4
ld   r3, YI + 4
in   r4, pins0
out  port0, r5
togl port1, r7
st   VI, r6
st   VI+3, r7
cmp  r7, -5
cmpc r7, 0
call proc
jz   begin
jl   begin
jne  begin
je   begin
jg   begin
jc   begin
jnz  begin
jnc  begin
jhc  begin
jnhc begin
stop
afcall proc2
jmp  begin

nop

proc:
xor r0, r0
ret

.org 100h
proc2:
add r0, r0
add r0, 10
ret

mov r1, r2
nop

push r0
pop  r1

mov  YL, LO(Hello)
mov  YH, HI(Hello)

end:
jmp end

.org 150h

.str Hello "Test string!"
.str Menu  "Menu"

.db Num 1

.def clr(reg) XOR reg, reg
.def inc(reg) ADD reg, 1
.def incc(reg) ADDC reg, 0
.def test()  STOP
.def push(r) ST SPD, r
.def pop(r)  LD r, SPI+1
