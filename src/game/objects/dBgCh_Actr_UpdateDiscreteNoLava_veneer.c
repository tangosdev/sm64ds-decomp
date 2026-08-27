/* dBgCh_Actr_UpdateDiscreteNoLava_veneer @ 0x2038420 (arm9) -- tail-call veneer to _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv (0x2037024).
 * ldr ip, [pc]; bx ip; .word 0x2037024
 */
extern void _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv(void);

void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void) {
    _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv();
}
