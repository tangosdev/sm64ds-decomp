/* func_0203f644 @ 0x203f644 (arm9) -- tail-call veneer to func_02040a94 (0x2040a94).
 * ldr ip, [pc]; bx ip; .word 0x2040a94
 */
extern void func_02040a94(void);

void func_0203f644(void) {
    func_02040a94();
}
