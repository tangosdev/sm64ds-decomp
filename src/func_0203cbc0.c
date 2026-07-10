/* func_0203cbc0 @ 0x203cbc0 (arm9) -- tail-call veneer to _ZdlPv (0x203cbf0).
 * ldr ip, [pc]; bx ip; .word 0x203cbf0
 */
extern void _ZdlPv(void);

void func_0203cbc0(void) {
    _ZdlPv();
}
