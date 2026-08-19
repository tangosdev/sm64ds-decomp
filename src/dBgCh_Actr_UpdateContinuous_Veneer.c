/* WithMeshClsn_UpdateContinuous_Veneer @ 0x20383fc (arm9) -- tail-call veneer to _ZN12WithMeshClsn16UpdateContinuousEv (0x20366b4).
 * ldr ip, [pc]; bx ip; .word 0x20366b4
 */
extern void _ZN12WithMeshClsn16UpdateContinuousEv(void);

void WithMeshClsn_UpdateContinuous_Veneer(void) {
    _ZN12WithMeshClsn16UpdateContinuousEv();
}
