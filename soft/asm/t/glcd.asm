entry:
  out CPU_FLAGS, 0x40 ; BF = 1
  mov SPL, 0xFF    ; init stack
  mov SPH, 0xFF
  mov VL, 0        ; init Video RAM ptr
  mov VH, 0
  out CPU_FLAGS, 0 ; BF = 0

  mov R0, 31       ; color and y counter
  xor R1, R1
b_lp:
  call line
  call line
  add R0, 0xFF     ; dec R0
  jnz b_lp

  mov R4, 31       ; color and y counter
  xor R1, R1
g_lp:
  mov R0, R4
  mul R0, 32       ; 5 bit left shift
  call line
  call line
  add R4, 0xFF     ; dec R4
  jnz g_lp

  mov R4, 31       ; color and y counter
  xor R0, R0
r_lp:
  mov R1, R4
  add R1, R1
  add R1, R1
  call line
  call line
  add R4, 0xFF     ; dec R4
  jnz r_lp

  stop

line:
  ; LCD width = 480, it's 0x1E0
  mov R2, LO(480)
  mov R3, HI(480)
x_lp:
  stv VI, R0       ; will write to video memory
  add R2, 0xFF     ; R3:R2--
  addc R3, 0xFF
  cmp R3, 0
  cmpc R2, 0
  jne x_lp

  ret
