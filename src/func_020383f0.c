/* func_020383f0 @ 0x20383f0 (arm9) -- tail-call veneer to _ZN10dBgCh_Actr22UpdateContinuousNoLavaEv (0x2036318).
 * ldr ip, [pc]; bx ip; .word 0x2036318
 * r0 (the dBgCh_Actr) passes straight through to the member.
 */
extern void _ZN10dBgCh_Actr22UpdateContinuousNoLavaEv(void *self);

void func_020383f0(void *self) {
    _ZN10dBgCh_Actr22UpdateContinuousNoLavaEv(self);
}
