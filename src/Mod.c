// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// LEFTOVER (unenrolled): 2-byte-aligned Thumb stub (addr % 4 == 2). Carving it as its own .text object pads to 4-byte section alignment and shifts the arm9 module: the Div trial linked 618528 bytes vs retail 618496 (+32) with 4624 mismatching functions. Per-function linkcheck VERIFIED; enroll when dsd supports 2-byte section starts.
#pragma thumb on
asm int Mod(int num, int den) {
    swi 0x09
    add r0, r1, #0
    bx lr
}
