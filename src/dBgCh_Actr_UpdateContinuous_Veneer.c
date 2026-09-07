/* dBgCh_Actr_UpdateContinuous_Veneer @ 0x20383fc (arm9) -- tail-call veneer to _ZN10dBgCh_Actr16UpdateContinuousEv (0x20366b4).
 * ldr ip, [pc]; bx ip; .word 0x20366b4
 */
extern void _ZN10dBgCh_Actr16UpdateContinuousEv(void);

void dBgCh_Actr_UpdateContinuous_Veneer(void) {
    _ZN10dBgCh_Actr16UpdateContinuousEv();
}
