/* func_0205b858 @ 0x205b858 (arm9) -- tail-call veneer to func_0205bad8 (0x205bad8).
 * ldr ip, [pc]; bx ip; .word 0x205bad8
 */
extern void func_0205bad8(void);

void func_0205b858(void) {
    func_0205bad8();
}
