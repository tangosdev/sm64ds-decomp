/* func_ov072_02121fa0 @ 0x2121fa0 (ov072) -- tail-call veneer to _ZN9ActorBase18MarkForDestructionEv (0x2043824).
 * ldr ip, [pc]; bx ip; .word 0x2043824
 */
extern void _ZN9ActorBase18MarkForDestructionEv(void);

void func_ov072_02121fa0(void) {
    _ZN9ActorBase18MarkForDestructionEv();
}
