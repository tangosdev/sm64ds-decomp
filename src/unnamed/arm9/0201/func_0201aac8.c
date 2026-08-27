/* func_0201aac8 @ 0x201aac8 (arm9) -- tail-call veneer to func_02059ba0 (0x2059ba0).
 * ldr ip, [pc]; bx ip; .word 0x2059ba0
 */
extern void func_02059ba0(void);

void func_0201aac8(void) {
    func_02059ba0();
}
