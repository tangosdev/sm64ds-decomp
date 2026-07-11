// Tail-call thunk: clear the word at +0x62dc, the byte at +0x62f4, and the
// halfword at +0x62f2, then tail-call SetBg2Offset(0). Hand-asm because the
// trailing .word is a relocation to the real target (a wildcard) and mwccarm
// would otherwise emit a `bl` with a stack frame instead of the ROM's pooled
// `ldr ip,[pc]; bx ip` tail-call.
extern void SetBg2Offset(void);

asm void func_ov006_020d5c60(void)
{
    add r2, r0, #0x6000
    mov r1, #0
    str r1, [r2, #0x2dc]
    add r3, r0, #0x6200
    strb r1, [r2, #0x2f4]
    ldr ip, [pc, #8]
    mov r0, r1
    strh r1, [r3, #0xf2]
    bx ip
    dcd SetBg2Offset
}
