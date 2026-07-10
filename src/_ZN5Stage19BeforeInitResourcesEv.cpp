//cpp
/* _ZN5Stage19BeforeInitResourcesEv @ 0x202ddc8 (arm9) -- tail-call veneer to _ZN5Scene19ResetFadersAndSoundEv (0x202e66c).
 * ldr ip, [pc]; bx ip; .word 0x202e66c
 */
extern "C" {
extern void _ZN5Scene19ResetFadersAndSoundEv(void);
void _ZN5Stage19BeforeInitResourcesEv(void) {
    _ZN5Scene19ResetFadersAndSoundEv();
}
}
