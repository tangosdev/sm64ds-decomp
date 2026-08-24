//cpp
// @symbol _ZN5dBgChC2Ev
/* recovered: real C++ constructor -- the vptr store is synthesized
 *
 * Renamed from func_02035514 (2026-08-23). RTTI on the vtable it stores
 * (data_020991d8 -> typeinfo "5dBgCh") names the class dBgCh, and all four
 * of its ROM callers are derived-constructor base steps (dBgCh_Actr,
 * dBgCh_Gnd, dBgCh_Lin, dBgCh_SphCrr -- each constructs dBgCh as its @0
 * subobject); nothing constructs a standalone dBgCh -- so the variant label
 * is C2. See notes/ctor-migration.md section 4/4b.
 *
 * The three stores after the vptr are this ctor's whole body; declaring
 * dBgCh() in include/dBgCh.h (already done for every child's base step)
 * is what makes them land as body work behind a synthesized vptr store.
 * The C1 sibling this definition also emits has no ROM counterpart and is
 * stripped by objisolate (section 1).
 */
#include "dBgCh.h"

dBgCh::dBgCh()
{
    unk_004 = 1;
    unk_00c = 0;
    unk_008 = -1;
}
