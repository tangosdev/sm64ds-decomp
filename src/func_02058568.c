// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
// OS_InitContext-style SDK primitive: stores entry pc+4 and the two stack pointers
// (sp_svc at +0x44, sp = sp_svc - 0x40 at +0x38), derives the initial CPSR from the
// Thumb bit of the entry address (SYS mode 0x1f, +T bit -> 0x3f), and clears r0-r12
// and lr in the context block. Same original hand-asm TU as ARMSaveContext (0x020585cc).
// C reproductions floor at a 5-word register-coloring residual (t_pc4/t_sp40 webs color
// r3/r1 under 1.2/sp2p3 for every source shape tried; ROM keeps both params in place).
asm void func_02058568(void *ctx, unsigned int pc, unsigned int sp)
{
    add r1, r1, #4
    str r1, [r0, #0x40]
    str r2, [r0, #0x44]
    sub r2, r2, #0x40
    str r2, [r0, #0x38]
    ands r1, r1, #1
    movne r1, #0x3f
    moveq r1, #0x1f
    str r1, [r0, #0]
    mov r1, #0
    str r1, [r0, #0x4]
    str r1, [r0, #0x8]
    str r1, [r0, #0xc]
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    str r1, [r0, #0x18]
    str r1, [r0, #0x1c]
    str r1, [r0, #0x20]
    str r1, [r0, #0x24]
    str r1, [r0, #0x28]
    str r1, [r0, #0x2c]
    str r1, [r0, #0x30]
    str r1, [r0, #0x34]
    str r1, [r0, #0x3c]
    bx lr
}
