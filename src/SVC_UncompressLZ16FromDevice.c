// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// Halfword-aligned Thumb stub. The link lowers this section alignment to 2 so the function keeps its address.
#pragma thumb on
asm void SVC_UncompressLZ16FromDevice(void) {
    swi 0x12
    bx lr
}
