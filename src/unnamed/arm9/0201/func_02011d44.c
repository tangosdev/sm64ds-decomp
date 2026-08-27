/* func_02011d44 @ 0x2011d44 (arm9) -- tail-call veneer to func_02048f14 (0x2048f14).
 * ldr ip, [pc]; bx ip; .word 0x2048f14
 */
extern void func_02048f14(void);

void func_02011d44(void) {
    func_02048f14();
}
