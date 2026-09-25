// @symbol daObjSm_Lift_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV14daObjSm_Lift_c[];
/* Reconstructed source-style name: SM64DS proves daObjSm_Lift_c through RTTI,
 * allocation size, vtable identity, and the SM_LIFT registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: SkiLift_Spawn. */
int *daObjSm_Lift_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(852);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV14daObjSm_Lift_c; }
    return p;
}
