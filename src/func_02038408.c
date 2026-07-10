/* func_02038408 @ 0x2038408 (arm9) -- tail-call veneer to func_02036acc (0x2036acc).
 * ldr ip, [pc]; bx ip; .word 0x2036acc
 */
extern void func_02036acc(void);

void func_02038408(void) {
    func_02036acc();
}
