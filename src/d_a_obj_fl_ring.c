// @symbol daObjFl_Ring_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV11VolcanoRing[];
/* Reconstructed source-style name: SM64DS proves daObjFl_Ring_c through RTTI,
 * allocation size, vtable identity, and the FL_RING registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's VolcanoRing implementation aliases remain unchanged.
 * Historical alias: VolcanoRing_Spawn. */
int *daObjFl_Ring_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV11VolcanoRing; }
    return p;
}
