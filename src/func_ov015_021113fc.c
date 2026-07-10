/* func_ov015_021113fc @ 0x21113fc (ov015) -- tail-call veneer to func_ov015_02111414 (0x2111414).
 * ldr ip, [pc]; bx ip; .word 0x2111414
 */
extern void func_ov015_02111414(void);

void func_ov015_021113fc(void) {
    func_ov015_02111414();
}
