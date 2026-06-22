extern void func_02059834(void);
asm void func_02059824(void) {
    stmdb sp!, {lr}
    bl func_02059834
    ldmia sp!, {lr}
    bx lr
}
