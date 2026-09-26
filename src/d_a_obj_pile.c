// @symbol daObjPile_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void *_ZN10dBgActor_cC2Ev(void *);
extern int _ZTV11daObjPile_c[];
/* Reconstructed source-style name: SM64DS proves daObjPile_c through RTTI,
 * allocation size, vtable identity, and the PILE registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: Stump_Spawn. */
int *daObjPile_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV11daObjPile_c; }
    return p;
}
