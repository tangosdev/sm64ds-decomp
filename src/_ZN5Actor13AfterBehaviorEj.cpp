//cpp
/* _ZN5Actor13AfterBehaviorEj @ 0x2010fc8 (arm9) -- tail-call veneer to _ZN9ActorBase13AfterBehaviorEj (0x2043af8).
 * ldr ip, [pc]; bx ip; .word 0x2043af8
 */
extern "C" {
extern void _ZN9ActorBase13AfterBehaviorEj(void);
void _ZN5Actor13AfterBehaviorEj(void) {
    _ZN9ActorBase13AfterBehaviorEj();
}
}
