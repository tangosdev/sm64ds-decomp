/* func_0201aaa4 @ 0x201aaa4 (arm9) -- tail-call veneer to func_02059950 (0x2059950).
 * ldr ip, [pc]; bx ip; .word 0x2059950
 */
extern void func_02059950(void);

void func_0201aaa4(void) {
    func_02059950();
}
