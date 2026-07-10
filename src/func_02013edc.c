/* func_02013edc @ 0x2013edc (arm9) -- tail-call veneer to func_02042fe4 (0x2042fe4).
 * ldr ip, [pc]; bx ip; .word 0x2042fe4
 */
extern void func_02042fe4(void);

void func_02013edc(void) {
    func_02042fe4();
}
