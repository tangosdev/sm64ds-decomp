/* func_02073220 @ 0x2073220 (arm9) -- tail-call veneer to func_0206dc4c (0x206dc4c).
 * ldr ip, [pc]; bx ip; .word 0x206dc4c
 */
extern void func_0206dc4c(void);

void func_02073220(void) {
    func_0206dc4c();
}
