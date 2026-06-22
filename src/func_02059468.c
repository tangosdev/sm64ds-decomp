extern void func_0205947c(void);
extern void func_0205950c(void);
asm void func_02059468(void) {
    stmdb sp!, {lr}
    bl func_0205947c
    bl func_0205950c
    ldmia sp!, {lr}
    bx lr
}
