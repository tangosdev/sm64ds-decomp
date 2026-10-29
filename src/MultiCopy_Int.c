// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// Copies len bytes (a multiple of 4) from src to dst, one word at a time.
asm void MultiCopy_Int(int *src, int *dst, int len)
{
    add ip, r1, r2
loop:
    cmp r1, ip
    ldmltia r0!, {r2}
    stmltia r1!, {r2}
    blt loop
    bx lr
}
