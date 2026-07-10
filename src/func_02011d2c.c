/* func_02011d2c @ 0x2011d2c (arm9) -- tail-call veneer to func_02048ef4 (0x2048ef4).
 * ldr ip, [pc]; bx ip; .word 0x2048ef4
 */
extern void func_02048ef4(void);

void func_02011d2c(void) {
    func_02048ef4();
}
