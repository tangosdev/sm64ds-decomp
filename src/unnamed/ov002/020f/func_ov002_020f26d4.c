/* func_ov002_020f26d4 @ 0x20f26d4 (ov002) -- tail-call veneer to func_ov002_020f2630 (0x20f2630).
 * ldr ip, [pc]; bx ip; .word 0x20f2630
 */
extern void func_ov002_020f2630(void);

void func_ov002_020f26d4(void) {
    func_ov002_020f2630();
}
