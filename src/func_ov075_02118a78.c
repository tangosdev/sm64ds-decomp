/* func_ov075_02118a78 @ 0x2118a78 (ov075) -- tail-call veneer to func_020308b4 (0x20308b4).
 * ldr ip, [pc]; bx ip; .word 0x20308b4
 */
extern void func_020308b4(void);

void func_ov075_02118a78(void) {
    func_020308b4();
}
