// @symbol _ZN5dBgChC2Ev
/* recovered: named members + shared header
 *
 * Renamed from func_02035514 (2026-08-23). RTTI on the vtable it stores
 * (data_020991d8 -> typeinfo "5dBgCh") names the class dBgCh, and all four
 * of its ROM callers are derived-constructor base steps (dBgCh_Actr,
 * dBgCh_Gnd, dBgCh_Lin, dBgCh_SphCrr -- each constructs dBgCh as its @0
 * subobject); nothing constructs a standalone dBgCh -- so the variant label
 * is C2. See notes/ctor-migration.md section 4/4b.
 */
extern int data_020991d8;
void _ZN5dBgChC2Ev(char *c){
  *(int**)c = &data_020991d8;
  *(unsigned char*)(c+4) = 1;
  *(int*)(c+0xc) = 0;
  *(int*)(c+8) = -1;
}
