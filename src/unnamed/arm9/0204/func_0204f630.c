/* func_0204f630 @ 0x204f630 (arm9) -- tail-call veneer to func_0204f2d4 (0x204f2d4).
 * ldr ip, [pc]; bx ip; .word 0x204f2d4
 */
extern void func_0204f2d4(void);

void func_0204f630(void) {
    func_0204f2d4();
}
