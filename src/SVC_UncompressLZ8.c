// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#pragma thumb on
#ifndef SM64DS_PLATFORM_PC
asm void SVC_UncompressLZ8(void) {
    swi 0x11
    bx lr
}
#else
void SVC_UncompressLZ8(void);
#endif
