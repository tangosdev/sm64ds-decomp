// @symbol daObjKm2_Agaru_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV15FireSeaElevator[];
/* Reconstructed source-style name: SM64DS proves daObjKm2_Agaru_c through its
 * ROM RTTI string, allocation size, vtable identity, and the KM2_AGARU
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. The project's FireSeaElevator implementation
 * alias remains unchanged. Historical alias: FireSeaElevator_Spawn. */
int *daObjKm2_Agaru_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV15FireSeaElevator; }
    return p;
}
