/* func_02011dc0 @ 0x2011dc0 (arm9) -- tail-call veneer to func_0204effc (0x204effc).
 * ldr ip, [pc]; bx ip; .word 0x204effc
 */
extern void func_0204effc(void);

void func_02011dc0(void) {
    func_0204effc();
}
