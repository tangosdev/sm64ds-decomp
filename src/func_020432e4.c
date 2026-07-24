// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM: ActorBase::Process PMF-arg staging + conditional func_0204302c(id).
// Below-sp mid-PMF pack via sub r3,sp,#4; ldmia r3,{r3}. Same floor as siblings.
extern int _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(void);
extern void func_0204302c(void);
extern void *data_02099e84;
extern void *data_02099e7c;
extern void *data_02099e8c;

asm int func_020432e4(void *self)
{
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #0xc
    ldr r1, [pc, #0x5c]
    ldrh r4, [r0, #0xc]
    ldr r3, [r1]
    ldr r2, [r1, #4]
    ldr r1, [pc, #0x50]
    str r3, [sp, #4]
    str r2, [sp, #8]
    ldr r2, [r1]
    ldr r1, [r1, #4]
    sub r3, sp, #4
    str r2, [r3]
    str r1, [r3, #4]
    ldr r1, [pc, #0x34]
    ldmia r3, {r3}
    ldmia r1, {r1, r2}
    bl _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE
    mov r5, r0
    cmp r5, #1
    bne Lskip
    mov r0, r4
    bl func_0204302c
Lskip:
    mov r0, r5
    add sp, sp, #0xc
    ldmia sp!, {r4, r5, lr}
    bx lr
    dcd data_02099e84
    dcd data_02099e7c
    dcd data_02099e8c
}
