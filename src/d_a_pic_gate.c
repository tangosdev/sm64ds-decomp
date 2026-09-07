extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern int _ZTV8Painting[];
/* Reconstructed source-style name: SM64DS proves daPicGate_c through RTTI,
 * allocation size, vtable identity, and the PICTURE_GATE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Painting_Spawn. */
int *daPicGate_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(444);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV8Painting; }
    return p;
}
