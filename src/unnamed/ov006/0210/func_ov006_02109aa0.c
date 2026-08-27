/* func_ov006_02109aa0 @ 0x2109aa0 (ov006) -- tail-call veneer to func_ov006_021079cc (0x21079cc).
 * ldr ip, [pc]; bx ip; .word 0x21079cc
 */
extern void func_ov006_021079cc(void);

void func_ov006_02109aa0(void) {
    func_ov006_021079cc();
}
