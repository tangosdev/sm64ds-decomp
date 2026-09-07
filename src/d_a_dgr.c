extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV7daDgr_c[];
/* Reconstructed source-style name: SM64DS proves daDgr_c through RTTI,
 * allocation size, vtable identity, and the DONGURU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov025_02111898. */
int *daDgr_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(820);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV7daDgr_c; }
    return p;
}
