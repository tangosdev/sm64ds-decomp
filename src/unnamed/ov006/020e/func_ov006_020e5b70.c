/* func_ov006_020e5b70 @ 0x20e5b70 (ov006) -- tail-call veneer to func_ov006_020e5450 (0x20e5450).
 * ldr ip, [pc]; bx ip; .word 0x20e5450
 */
extern void func_ov006_020e5450(void);

void func_ov006_020e5b70(void) {
    func_ov006_020e5450();
}
