/* dBgCh_Actr_UpdateContinuous_Veneer @ 0x20383fc (arm9) -- tail-call veneer to _ZN10dBgCh_Actr16UpdateContinuousEv (0x20366b4).
 * ldr ip, [pc]; bx ip; .word 0x20366b4
 * r0 (the dBgCh_Actr) passes straight through to the member.
 */
extern void _ZN10dBgCh_Actr16UpdateContinuousEv(void *self);

void dBgCh_Actr_UpdateContinuous_Veneer(void *self) {
    _ZN10dBgCh_Actr16UpdateContinuousEv(self);
}
