// @symbol daObjTdFuta_c_classInit
/* Factory remains in its measured C ABI form. mwccarm rejects fBase_c's actor
 * allocator as an in-class operator new declaration (see fBase_c.h), so a
 * source-level `new daObjTdFuta_c` binds the wrong global allocator. The ROM calls
 * the actor allocator, base constructor, and derived vptr store explicitly. */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV13daObjTdFuta_c[];
/* Reconstructed source-style name: SM64DS proves daObjTdFuta_c through RTTI,
 * allocation size, vtable identity, and the TD_FUTA registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's daObjTdFuta_c implementation alias remains unchanged.
 * Historical alias: HugeCover_Spawn. */
int *daObjTdFuta_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV13daObjTdFuta_c; }
    return p;
}
