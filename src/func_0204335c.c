// HAND-ASM: sibling of func_0204322c — ActorBase::Process PMF-arg staging.
// Pool: data_02099e94 (3rd), data_02099ec4 (2nd), data_02099ebc (1st).
extern int _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(void);
extern void *data_02099e94;
extern void *data_02099ec4;
extern void *data_02099ebc;

asm void func_0204335c(void *self)
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
    dcd data_02099e94
    dcd data_02099ec4
    dcd data_02099ebc
}
