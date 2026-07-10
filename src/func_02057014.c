// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Veneer that tail-calls
// func_02057178 through r1 (compiled C always uses ip for tail-call veneers,
// so this register choice marks hand-written assembly).
extern void func_02057178(void);

asm void func_02057014(void) {
    ldr r1, =func_02057178
    bx r1
}
