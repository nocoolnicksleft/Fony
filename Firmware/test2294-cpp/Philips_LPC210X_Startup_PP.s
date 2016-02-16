# 1 "C:/LokaleDaten/ARMprojects/test2294-cpp/Philips_LPC210X_Startup.s"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "C:/LokaleDaten/ARMprojects/test2294-cpp/Philips_LPC210X_Startup.s"
# 121 "C:/LokaleDaten/ARMprojects/test2294-cpp/Philips_LPC210X_Startup.s"
  .section .vectors, "ax"
  .code 32
  .align 0
  .global _vectors
  .global reset_handler




_vectors:
  ldr pc, [pc, #reset_handler_address - . - 8]
  ldr pc, [pc, #undef_handler_address - . - 8]
  ldr pc, [pc, #swi_handler_address - . - 8]
  ldr pc, [pc, #pabort_handler_address - . - 8]
  ldr pc, [pc, #dabort_handler_address - . - 8]

  .word 0xB9205F88
  ldr pc, [pc, #-0xFF0]




  ldr pc, [pc, #fiq_handler_address - . - 8]

reset_handler_address:



  .word reset_wait

undef_handler_address:
  .word undef_handler
swi_handler_address:
  .word swi_handler
pabort_handler_address:
  .word pabort_handler
dabort_handler_address:
  .word dabort_handler




fiq_handler_address:
  .word fiq_handler

  .section .init, "ax"
  .code 32
  .align 0






reset_handler:
  ldr r0, =0xE01FC000



  ldr r1, =0x23
  str r1, [r0, #0x084]

  mov r1, #0x1
  str r1, [r0, #0x080]
  mov r1, #0xAA
  str r1, [r0, #0x08C]
  mov r1, #0x55
  str r1, [r0, #0x08C]

pll_lock_loop:
  ldr r1, [r0, #0x088]
  tst r1, #0x400
  beq pll_lock_loop

  mov r1, #0x3
  str r1, [r0, #0x080]
  mov r1, #0xAA
  str r1, [r0, #0x08C]
  mov r1, #0x55
  str r1, [r0, #0x08C]



  mov r1, #0
  str r1, [r0, #0x000]
  ldr r1, =3
  str r1, [r0, #0x004]
  ldr r1, =2
  str r1, [r0, #0x000]


  ldr r1, =2
  str r1, [r0, #0x100]
# 268 "C:/LokaleDaten/ARMprojects/test2294-cpp/Philips_LPC210X_Startup.s"
  b _start


reset_wait:
  b reset_wait
# 281 "C:/LokaleDaten/ARMprojects/test2294-cpp/Philips_LPC210X_Startup.s"
undef_handler:
  b undef_handler

swi_handler:
  b swi_handler

pabort_handler:
  b pabort_handler

dabort_handler:
  b dabort_handler

fiq_handler:
  b fiq_handler

irq_handler:
  b irq_handler

  .weak undef_handler, swi_handler, pabort_handler, dabort_handler, fiq_handler, irq_handler
