// @symbol _ZN10dScEntry_c19BeforeInitResourcesEv
/* dScEntry_c::BeforeInitResources() -- vtable slot 1. Pure tail-call veneer
 * to dScene_c::ResetFadersAndSound (0x202e66c) -- ldr ip, [pc]; bx ip;
 * .word 0x202e66c. Plain C function carrying the literal mangled name --
 * see include/dScEntry_c.h. */
extern void _ZN8dScene_c19ResetFadersAndSoundEv(void);

void _ZN10dScEntry_c19BeforeInitResourcesEv(void) {
    _ZN8dScene_c19ResetFadersAndSoundEv();
}
