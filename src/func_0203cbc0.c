/* func_0203cbc0 @ 0x203cbc0 (arm9) -- tail-call veneer to _ZdlPv (0x203cbf0).
 * ldr ip, [pc]; bx ip; .word 0x203cbf0
 *
 * Takes the block to free and hands it on in r0. A tail call leaves r0 alone,
 * so the old (void) spelling matched as well.
 */
extern void _ZdlPv(void *ptr);

void func_0203cbc0(void *ptr) {
    _ZdlPv(ptr);
}
