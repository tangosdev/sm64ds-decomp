/* func_02011d20 @ 0x2011d20 (arm9) -- tail-call veneer to _ZN5Sound13Func_02048ee4Ev (0x2048ee4).
 * ldr ip, [pc]; bx ip; .word 0x2048ee4
 */
extern void _ZN5Sound13Func_02048ee4Ev(void);

void func_02011d20(void) {
    _ZN5Sound13Func_02048ee4Ev();
}
