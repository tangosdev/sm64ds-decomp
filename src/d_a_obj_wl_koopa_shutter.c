// @symbol daObjWlKoopaShutter_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV13BowserShutter[];
/* Reconstructed source-style name: SM64DS proves daObjWlKoopaShutter_c through
 * RTTI, allocation size, vtable identity, and the WL_KOOPA_SHUTTER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's BowserShutter implementation aliases remain
 * unchanged. Historical alias: BowserShutter_Spawn. */
int *daObjWlKoopaShutter_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV13BowserShutter; }
    return p;
}
