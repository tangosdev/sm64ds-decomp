/* func_02011d50 @ 0x2011d50 (arm9) -- tail-call veneer to func_02048f24 (0x2048f24).
 * ldr ip, [pc]; bx ip; .word 0x2048f24
 */
extern void func_02048f24(void);

void func_02011d50(void) {
    func_02048f24();
}
