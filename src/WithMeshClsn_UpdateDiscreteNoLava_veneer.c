/* WithMeshClsn_UpdateDiscreteNoLava_veneer @ 0x2038420 (arm9) -- tail-call veneer to _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv (0x2037024).
 * ldr ip, [pc]; bx ip; .word 0x2037024
 */
extern void _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv(void);

void WithMeshClsn_UpdateDiscreteNoLava_veneer(void) {
    _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv();
}
