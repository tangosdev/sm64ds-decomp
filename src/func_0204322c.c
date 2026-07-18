// HAND-ASM: ActorBase::Process PMF-arg staging via below-sp ldm (sub ip,sp,#4;
// ldmia ip,{r3}; ldmia r1,{r1,r2}). Same softfloat/SDK shape documented in
// notes/mwccarm-codegen.md sec 8 / matched func_0206ddcc family — no mwccarm
// C/C++ under -O4,p emits below-sp staging without a frame-pointer adjustment.
// Pool order (third, second, first PMF) matches ROM at 0x0204322c.
extern int _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(void);
extern void *data_02099e9c;
extern void *data_02099e74;
extern void *data_02099ecc;

asm void func_0204322c(void *self)
{
    stmdb sp!, {lr}
    sub sp, sp, #0xc
    ldr r2, [pc, #0x40]
    ldr r1, [pc, #0x40]
    ldr r3, [r2]
    ldr r2, [r2, #4]
    sub ip, sp, #4
    str r3, [sp, #4]
    str r2, [sp, #8]
    ldr r3, [r1]
    ldr r2, [r1, #4]
    ldr r1, [pc, #0x24]
    str r3, [ip]
    str r2, [ip, #4]
    ldmia ip, {r3}
    ldmia r1, {r1, r2}
    bl _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE
    add sp, sp, #0xc
    ldmia sp!, {lr}
    bx lr
    dcd data_02099e9c
    dcd data_02099e74
    dcd data_02099ecc
}
