// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
/* The ARM semihosting SYS_EXIT call (r0 = 0x18); see sys_writec. */
#ifndef SM64DS_PLATFORM_PC
asm void sys_exit(void) {
    mov r1, #0
    mov r0, #0x18
    swi 0x123456
    mov pc, lr
}
#else
void sys_exit(void);
#endif
