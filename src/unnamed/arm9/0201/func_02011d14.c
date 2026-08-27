/* func_02011d14 @ 0x2011d14 (arm9) -- tail-call veneer to func_02048ed4 (0x2048ed4).
 * ldr ip, [pc]; bx ip; .word 0x2048ed4
 */
extern void func_02048ed4(void);

void func_02011d14(void) {
    func_02048ed4();
}
