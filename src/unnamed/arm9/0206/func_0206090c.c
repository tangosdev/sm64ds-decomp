/* func_0206090c @ 0x206090c (arm9) -- tail-call veneer to func_0205ffd0 (0x205ffd0).
 * ldr ip, [pc]; bx ip; .word 0x205ffd0
 */
extern void func_0205ffd0(void);

void func_0206090c(void) {
    func_0205ffd0();
}
