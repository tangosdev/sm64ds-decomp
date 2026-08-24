// @symbol _ZN5dBgPiC2Ev
/* recovered: named members + shared header
 *
 * Renamed from func_0203819c (2026-08-23). RTTI on the vtable it stores
 * (data_02099368 -> typeinfo "5dBgPi") names the class dBgPi, and all three
 * of its ROM callers are derived-constructor base steps (inside dBgCh_Gnd,
 * dBgCh_Lin and dBgCh_SphCrr, each constructing dBgPi at their +0x10
 * subobject) -- so the variant label is C2. See notes/ctor-migration.md
 * section 4/4b.
 */
extern void _ZN5dBgPcC2Ev(void *);
extern void func_020380c0(void *);
extern int data_02099368[];
int *_ZN5dBgPiC2Ev(int *t)
{
    _ZN5dBgPcC2Ev((char *)t + 0x4);
    t[0] = (int)data_02099368;
    func_020380c0(t);
    return t;
}
