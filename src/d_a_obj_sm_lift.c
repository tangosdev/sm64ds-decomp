// @symbol daObjSm_Lift_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV7SkiLift[];
/* Reconstructed source-style name: SM64DS proves daObjSm_Lift_c through RTTI,
 * allocation size, vtable identity, and the SM_LIFT registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's SkiLift implementation aliases remain unchanged. Historical
 * alias: SkiLift_Spawn. */
int *daObjSm_Lift_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(852);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV7SkiLift; }
    return p;
}
