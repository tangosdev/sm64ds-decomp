//cpp
/* _ZN6Memory16operator_delete2EPv @ 0x203cbcc (arm9) -- tail-call veneer to _ZdlPv (0x203cbf0).
 * ldr ip, [pc]; bx ip; .word 0x203cbf0
 */
extern "C" {
extern void _ZdlPv(void);
void _ZN6Memory16operator_delete2EPv(void) {
    _ZdlPv();
}
}
