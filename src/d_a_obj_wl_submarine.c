// @symbol daObjWlSubmarine_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV9Submarine[];
/* Reconstructed source-style name: SM64DS proves daObjWlSubmarine_c through
 * RTTI, allocation size, vtable identity, and the WL_SUBMARINE registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's Submarine implementation aliases remain
 * unchanged. Historical alias: Submarine_Spawn. */
int *daObjWlSubmarine_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV9Submarine; }
    return p;
}
