/* func_ov079_02123b54 @ 0x2123b54 (ov079) -- tail-call veneer to func_ov079_02123d4c (0x2123d4c).
 * ldr ip, [pc]; bx ip; .word 0x2123d4c
 */
extern void func_ov079_02123d4c(void);

void func_ov079_02123b54(void) {
    func_ov079_02123d4c();
}
