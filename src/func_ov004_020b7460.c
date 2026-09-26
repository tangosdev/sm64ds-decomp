/* func_ov004_020b7460 @ 0x20b7460 (ov004) -- tail-call veneer to func_ov004_020b743c (0x20b743c).
 * ldr ip, [pc]; bx ip; .word 0x20b743c
 *
 * The object pointer both callers pass in r0 rides through to the target, whose
 * first store is through it (+0x1c = 10), so it is spelled as the parameter it is.
 */
extern void func_ov004_020b743c(char *p);

void func_ov004_020b7460(void *c) {
    func_ov004_020b743c(c);
}
