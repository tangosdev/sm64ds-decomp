/* func_ov006_02110bb4 @ 0x2110bb4 (ov006) -- tail-call veneer to func_ov006_02114738 (0x2114738).
 * ldr ip, [pc]; bx ip; .word 0x2114738
 */
extern void func_ov006_02114738(void);

void func_ov006_02110bb4(void) {
    func_ov006_02114738();
}
