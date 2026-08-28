/* func_02013edc @ 0x2013edc (arm9) -- tail-call veneer to func_02042fe4 (0x2042fe4).
 * ldr ip, [pc]; bx ip; .word 0x2042fe4
 */
extern int func_02042fe4(int, int, int);

int func_02013edc(int a, int b, int c) {
    return func_02042fe4(a, b, c);
}
