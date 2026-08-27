/* func_020427f8 @ 0x20427f8 (arm9) -- tail-call veneer to func_02059788 (0x2059788).
 * ldr ip, [pc]; bx ip; .word 0x2059788
 */
extern void func_02059788(void);

void func_020427f8(void) {
    func_02059788();
}
