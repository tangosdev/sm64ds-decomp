/* func_02011cfc @ 0x2011cfc (arm9) -- tail-call veneer to _ZN5Sound13Func_02048eb4Ev (0x2048eb4).
 * ldr ip, [pc]; bx ip; .word 0x2048eb4
 */
extern void _ZN5Sound13Func_02048eb4Ev(void);

void func_02011cfc(void) {
    _ZN5Sound13Func_02048eb4Ev();
}
