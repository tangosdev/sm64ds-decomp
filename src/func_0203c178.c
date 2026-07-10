/* func_0203c178 @ 0x203c178 (arm9) -- tail-call veneer to func_020527e9 (0x20527e9).
 * ldr ip, [pc]; bx ip; .word 0x20527e9
 */
extern void func_020527e9(void);

void func_0203c178(void) {
    func_020527e9();
}
