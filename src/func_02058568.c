// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. Nintendo shipped this as an
// assembly primitive, so there is no original C to recover and no match to chase. Counts as
// done under the asm-primitive policy - see notes/arm9-endgame.md.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
// This is NitroSDK OS_InitContext, and the "-style" hedge can come off: Nintendo's own SDK
// source writes it as assembly. OS_context.c carries
// `asm void OS_InitContext(register OSContext *context, register u32 newpc, register u32 newsp)`
// with this instruction sequence, the only difference being a later SDK revision's extra
// `tst newsp,#4 / subne newsp,newsp,#4` 8-byte-alignment fixup, which this build predates.
// The two functions that follow it here, ARMSaveContext (0x020585cc) and ARMRestoreContext
// (0x02058618), are OS_SaveContext and OS_RestoreContext out of the same TU and are `asm`
// there too; both are already accepted primitives in this tree. That is direct evidence for
// the first row of notes/asm-policy.md's table (the original was hand-written asm), not an
// inference from the instruction set.
//
// What it does: stores entry pc+4 at +0x40 and the two stack pointers (sp_svc at +0x44,
// sp = sp_svc - HW_SVC_STACK_SIZE (0x40) at +0x38), derives the initial CPSR from the Thumb
// bit of the entry address (SYS mode 0x1f, +T bit -> 0x3f), and clears r0-r12 and lr.
//
// C reproductions floor at a 5-word register-coloring residual and the floor was re-measured
// under 2004/b56 (18 source shapes): ROM keeps both pointer parameters in their own incoming
// registers (`add r1,r1,#4` / `sub r2,r2,#0x40`, then `ands r1,r1,#1` in place); mwccarm
// rotates them to r3 and r1 whenever the pc+4 value stays live past the sp store. Inert:
// in-place `+=` vs named locals, either declaration order, `register` on the parameters,
// u32 vs pointer parameters, a dead 4th/5th argument to occupy r3, `&end[-0x40]`, embedded
// assignments, a named ternary temp, void* parameters. Dropping the later use of pc+4 does
// color it r1, which localises the residual to that one live range.
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
