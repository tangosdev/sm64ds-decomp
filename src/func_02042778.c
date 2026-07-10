/* func_02042778 @ 0x2042778 (arm9) -- tail-call veneer to func_0201a4d0 (0x201a4d0).
 * ldr ip, [pc]; bx ip; .word 0x201a4d0
 */
extern void func_0201a4d0(void);

void func_02042778(void) {
    func_0201a4d0();
}
