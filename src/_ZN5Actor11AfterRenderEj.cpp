//cpp
/* _ZN5Actor11AfterRenderEj @ 0x2010f6c (arm9) -- tail-call veneer to _ZN9ActorBase11AfterRenderEj (0x2043ac4).
 * ldr ip, [pc]; bx ip; .word 0x2043ac4
 */
extern "C" {
extern void _ZN9ActorBase11AfterRenderEj(void);
void _ZN5Actor11AfterRenderEj(void) {
    _ZN9ActorBase11AfterRenderEj();
}
}
