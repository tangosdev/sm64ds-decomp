/* func_0203c178 @ 0x203c178 (arm9) -- tail-call veneer to func_020527e8 (0x20527e8).
 * ldr ip, [pc]; bx ip; .word 0x20527e9 (Thumb bit set on the BX target; the
 * real symbol, config/arm9/symbols.txt, is func_020527e8 thumb size 0x16)
 */
extern void func_020527e8(void);

void func_0203c178(void) {
    func_020527e8();
}
