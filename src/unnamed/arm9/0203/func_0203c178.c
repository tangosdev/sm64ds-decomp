/* func_0203c178 @ 0x203c178 (arm9) -- tail-call veneer to func_020527e8 (0x20527e8).
 * ldr ip, [pc]; bx ip; .word 0x20527e9 (Thumb bit set on the BX target; the
 * real symbol, config/arm9/symbols.txt, is func_020527e8 thumb size 0x16)
 */
extern void func_020527e8(int *m, int sx, int sy, int sz);

/* (m, sx, sy, sz) pass straight through in r0-r3 to the Thumb body, which
   builds the 4x3 scale matrix (NitroSDK's MTX_Scale43_ shape). */
void func_0203c178(void *m, int sx, int sy, int sz) {
    func_020527e8(m, sx, sy, sz);
}
