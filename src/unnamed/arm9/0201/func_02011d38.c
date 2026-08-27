/* func_02011d38 @ 0x2011d38 (arm9) -- tail-call veneer to func_02048f04 (0x2048f04).
 * ldr ip, [pc]; bx ip; .word 0x2048f04
 */
extern void func_02048f04(void);

void func_02011d38(void) {
    func_02048f04();
}
