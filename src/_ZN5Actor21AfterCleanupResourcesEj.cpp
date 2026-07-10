//cpp
/* _ZN5Actor21AfterCleanupResourcesEj @ 0x2011214 (arm9) -- tail-call veneer to _ZN9ActorBase21AfterCleanupResourcesEj (0x2043b2c).
 * ldr ip, [pc]; bx ip; .word 0x2043b2c
 */
extern "C" {
extern void _ZN9ActorBase21AfterCleanupResourcesEj(void);
void _ZN5Actor21AfterCleanupResourcesEj(void) {
    _ZN9ActorBase21AfterCleanupResourcesEj();
}
}
