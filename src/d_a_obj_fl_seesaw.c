// @symbol daObjFl_Seesaw_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV10LavaSeesaw[];
/* Reconstructed source-style name: SM64DS proves daObjFl_Seesaw_c through RTTI,
 * allocation size, vtable identity, and the FL_SEESAW registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's LavaSeesaw implementation aliases remain unchanged.
 * Historical alias: LavaSeesaw_Spawn. */
int *daObjFl_Seesaw_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV10LavaSeesaw; }
    return p;
}
