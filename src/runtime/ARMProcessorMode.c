// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (CPSR read), so there is no C to decompile it to -- the asm
// block is the faithful source. Counts as matched (asm-primitive policy), not a
// C transcription.
asm void ARMProcessorMode(void) { mrs r0, cpsr; and r0, r0, #0x1f; bx lr }
