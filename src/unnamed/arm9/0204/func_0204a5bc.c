/* func_0204a5bc @ 0x204a5bc (arm9) -- tail-call veneer to func_0204c584 (0x204c584).
 * ldr ip, [pc]; bx ip; .word 0x204c584
 *
 * Forwards both of its arguments in r0 and r1. A tail call leaves them alone,
 * so the old (void) spelling matched as well.
 */
extern void func_0204c584(void *self, void *list);

void func_0204a5bc(void *self, void *list) {
    func_0204c584(self, list);
}
