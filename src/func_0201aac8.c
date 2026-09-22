/* func_0201aac8 @ 0x201aac8 (arm9) -- tail-call veneer to func_02059ba0 (0x2059ba0).
 * ldr ip, [pc]; bx ip; .word 0x2059ba0
 * recovered: the veneer never touches r0, so whatever this function was called
 * with rides straight into func_02059ba0(char *p). func_0201aad4 already passes
 * one argument (through a K&R extern), which is why the empty parameter list
 * was byte-exact and wrong.
 */
extern void func_02059ba0(char *p);

void func_0201aac8(char *p) {
    func_02059ba0(p);
}
