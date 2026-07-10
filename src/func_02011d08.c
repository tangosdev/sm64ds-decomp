/* func_02011d08 @ 0x2011d08 (arm9) -- tail-call veneer to _ZN5Sound13Func_02048ec4Ev (0x2048ec4).
 * ldr ip, [pc]; bx ip; .word 0x2048ec4
 */
extern void _ZN5Sound13Func_02048ec4Ev(void);

void func_02011d08(void) {
    _ZN5Sound13Func_02048ec4Ev();
}
