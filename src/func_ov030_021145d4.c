/* func_ov030_021145d4 @ 0x21145d4 (ov030) -- tail-call veneer to _ZN9ActorBase18MarkForDestructionEv (0x2043824).
 * ldr ip, [pc]; bx ip; .word 0x2043824
 */
extern void _ZN9ActorBase18MarkForDestructionEv(void);

void func_ov030_021145d4(void) {
    _ZN9ActorBase18MarkForDestructionEv();
}
