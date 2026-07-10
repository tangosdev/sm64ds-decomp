//cpp
/* _ZN5Scene18AfterInitResourcesEj @ 0x202e62c (arm9) -- tail-call veneer to _ZN12ActorDerived18AfterInitResourcesEj (0x2013ef4).
 * ldr ip, [pc]; bx ip; .word 0x2013ef4
 */
extern "C" {
extern void _ZN12ActorDerived18AfterInitResourcesEj(void);
void _ZN5Scene18AfterInitResourcesEj(void) {
    _ZN12ActorDerived18AfterInitResourcesEj();
}
}
