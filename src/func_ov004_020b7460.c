/* func_ov004_020b7460 @ 0x20b7460 (ov004) -- tail-call veneer to func_ov004_020b743c (0x20b743c).
 * ldr ip, [pc]; bx ip; .word 0x20b743c
 */
extern void func_ov004_020b743c(void);

void func_ov004_020b7460(void) {
    func_ov004_020b743c();
}
