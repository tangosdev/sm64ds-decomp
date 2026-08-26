// @symbol HugeCover_Spawn
/* Factory remains in its measured C ABI form. mwccarm rejects fBase_c's actor
 * allocator as an in-class operator new declaration (see fBase_c.h), so a
 * source-level `new HugeCover` binds the wrong global allocator. The ROM calls
 * the actor allocator, base constructor, and derived vptr store explicitly. */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV9HugeCover[];
int *HugeCover_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV9HugeCover; }
    return p;
}
