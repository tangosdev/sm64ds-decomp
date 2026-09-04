// @symbol daObjLava_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern int _ZTV18PoppingLavaBubbles[];
/* Reconstructed source-style name: SM64DS proves daObjLava_c through RTTI,
 * allocation size, vtable identity, and the LAVA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: PoppingLavaBubbles_Spawn. */
int *daObjLava_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV18PoppingLavaBubbles; }
    return p;
}
