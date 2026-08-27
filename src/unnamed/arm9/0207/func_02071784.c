/* func_02071784 @ 0x2071784 (arm9) -- tail-call veneer to func_0207199c (0x207199c).
 * ldr ip, [pc]; bx ip; .word 0x207199c
 */
extern void func_0207199c(void);

void func_02071784(void) {
    func_0207199c();
}
