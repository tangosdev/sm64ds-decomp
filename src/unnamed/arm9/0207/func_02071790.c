// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
//
// C++ exception runtime: enter a landing pad. r0 is the throw context (for
// example the 0x70-byte block __rethrow builds), r1 the action iterator, and r2 the pad
// address. func_02071be8 and func_02071f1c both pass a function start plus a
// PC offset. The function reloads r4-r11 from the context's saved registers
// at 0x2c..0x48, sets sp to the word at 0x5c minus the word at 0x64, and
// branches. __rethrow stores the caller's sp at 0x5c, and func_02071864 fills
// 0x64 from the frame header. r0 and r1 reach the pad unchanged.
struct ThrowContext;
struct ActionIterator;

asm void func_02071790(struct ThrowContext *context, struct ActionIterator *iterator, int pad) {
    ldr r4, [r0, #0x2c]
    ldr r5, [r0, #0x30]
    ldr r6, [r0, #0x34]
    ldr r7, [r0, #0x38]
    ldr r8, [r0, #0x3c]
    ldr r9, [r0, #0x40]
    ldr r10, [r0, #0x44]
    ldr r11, [r0, #0x48]
    ldr sp, [r0, #0x5c]
    ldr r12, [r0, #0x64]
    sub sp, sp, r12
    mov pc, r2
}
