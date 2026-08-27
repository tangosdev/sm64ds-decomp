/* func_0204a5bc @ 0x204a5bc (arm9) -- tail-call veneer to func_0204c584 (0x204c584).
 * ldr ip, [pc]; bx ip; .word 0x204c584
 */
extern void func_0204c584(void);

void func_0204a5bc(void) {
    func_0204c584();
}
