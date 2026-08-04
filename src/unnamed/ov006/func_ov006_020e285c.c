/* func_ov006_020e285c @ 0x20e285c (ov006) -- tail-call veneer to func_ov006_020e20bc (0x20e20bc).
 * ldr ip, [pc]; bx ip; .word 0x20e20bc
 */
extern void func_ov006_020e20bc(void);

void func_ov006_020e285c(void) {
    func_ov006_020e20bc();
}
