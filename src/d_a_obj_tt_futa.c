// @symbol daObjTtFuta_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV9TinyCover[];
/* Reconstructed source-style name: SM64DS proves daObjTtFuta_c through its
 * RTTI string, allocation size, vtable identity, and the TT_FUTA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's TinyCover implementation alias remains
 * unchanged. Historical alias: TinyCover_Spawn. */
int *daObjTtFuta_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV9TinyCover; }
    return p;
}
