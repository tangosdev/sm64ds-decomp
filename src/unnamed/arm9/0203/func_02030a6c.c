/* func_02030a6c @ 0x2030a6c (arm9) -- tail-call veneer to func_0203d8e8 (0x203d8e8).
 * ldr ip, [pc]; bx ip; .word 0x203d8e8
 */
extern void _Z13func_0203d8e8v(void);

void func_02030a6c(void) {
    _Z13func_0203d8e8v();
}
