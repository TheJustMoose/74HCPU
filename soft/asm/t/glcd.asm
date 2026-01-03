entry:
  out CPU_FLAGS, 0x40 ; BF = 1
  mov SPL, 0xFF    ; init stack
  mov SPH, 0xFF
  mov XL, 0        ; init Video RAM ptr
  mov XH, 0
  out CPU_FLAGS, 0 ; BF = 0

  mov R0, 31       ; color and y counter
  xor R1, R1
y_lp:
  ; LCD width = 480, it's 0x1E0
  mov R2, LO(480)
  mov R3, HI(480)
x_lp:
  stv XI, R0       ; will write to video memory
  add R2, 0xFF     ; R3:R2--
  addc R3, 0xFF
  cmp R3, 0
  cmpc R2, 0
  jne x_lp

  add R0, 0xFF     ; dec R0
  jnz y_lp

  stop
