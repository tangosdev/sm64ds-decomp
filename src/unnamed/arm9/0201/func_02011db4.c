/* func_02011db4 @ 0x2011db4 (arm9) -- tail-call veneer to func_0204efe0 (0x204efe0).
 * ldr ip, [pc]; bx ip; .word 0x204efe0
 */
extern void func_0204efe0(void);

void func_02011db4(void) {
    func_0204efe0();
}
