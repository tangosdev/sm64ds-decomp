// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
/* The ARM semihosting SYS_WRITEC call (r0 = 3), one of the three stubs of
   the runtime's semihosted_console_io.c (pret pokediamond
   arm9/asm/RUNTIME_ARM_semihosted_console_io.s). */
asm void sys_writec(const unsigned char *c) {
    str lr, [sp, #-4]!
    mov r1, r0
    mov r0, #3
    swi 0x123456
    ldr pc, [sp], #4
}
