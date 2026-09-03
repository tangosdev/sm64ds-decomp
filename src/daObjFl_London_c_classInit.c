// @symbol daObjFl_London_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV16daObjFl_London_c[];
/* Reconstructed source-style name: SM64DS proves daObjFl_London_c through RTTI,
 * allocation size, vtable identity, and the FL_LONDON registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's daObjFl_London_c implementation aliases remain unchanged.
 * Historical alias: daObjFl_London_c_Spawn. */
int *daObjFl_London_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV16daObjFl_London_c; }
    return p;
}
