//cpp
/* Wet-Dry World's spinning disc -- ov029/daObjWc_Obj07_c, 5 functions.
 *
 * ENROLLED AND CANONICAL. This is a genuine translation unit: one .text run,
 * 0x02112080..0x021121a4, linked from this file alone. It replaced five
 * one-function sources, and the build reports 106/106 exact, 100.000000%,
 * mismatching 0.
 *
 * IT IS ALSO THE CLASS'S KEY-FUNCTION TU, which is why the RTTI rename came
 * first. With ~daObjWc_Obj07_c() inline in the header the key function is the
 * first DECLARED non-inline virtual -- CleanupResources -- so mwccarm emits
 * the _ZTV/_ZTI/_ZTS group here as vague linkage. All thirteen such records
 * are licensed in config/tu_manifest.d/ov029/daObjWc_Obj07_c.json as
 * deadstrip-data with a canonical module and address, and romdata_check
 * word-compares each against the cartridge: 7 VERIFIED, 6 PARTIAL, 0 DIFFERS.
 * _ZTV15daObjWc_Obj07_c is VERIFIED at 128 bytes, and 128 is the full span of
 * the table's SLOTS rather than a comfortable prefix: the address point is
 * ov029 0x02114018 and the next symbol, _ZTI14daObjWc_Mizu_c, begins at
 * 0x02114098. The slots are neither truncated nor overrun. What that does NOT
 * cover is the 8-byte {offset-to-top, _ZTI pointer} header at 0x02114010,
 * which is emitted but which no word comparison in the tree reaches -- the
 * comparison starts at the address point, by construction.
 * The 6 PARTIAL are only _ZTS strings, the known
 * benign pattern. None of this was reachable under the coined name
 * "RotatingPlatformWdw": these records are length-prefixed mangled strings,
 * so they could not have matched at any address.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02112080  src/_ZN15daObjWc_Obj07_cD1Ev.cpp
 *   [1] 0x021120d0  src/_ZN15daObjWc_Obj07_cD0Ev.cpp
 *   [2] 0x02112134  src/_ZN15daObjWc_Obj07_c16CleanupResourcesEv.cpp
 *   [3] 0x02112148  src/_ZN15daObjWc_Obj07_c13InitResourcesEv.cpp
 *   [4] 0x02112168  src/daObjWc_Obj07_c_Spawn.c
 *
 * tubuild's create pass left one #pragma uncarried and asked for a decision
 * by hand; it was resolved at CleanupResources below, where the reasoning
 * lives next to the code it governs.
 */

#include "daObjWc_Obj07_c.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

/* What is left of tubuild's shadow block, after reconciling it against the
 * real headers. func_ov002_020b66a8 and data_ov029_02113fc4 are declared by
 * include/decl_common.h, included above, so the duplicate local copies are
 * gone; _ZTV10dBgActor_c and data_020a0eac were carried in from a legacy file
 * that no longer contributes a body here and were referenced by nothing.
 *
 * The two that stay are the two no header declares. `Arg` is still a shadow
 * and is deliberately not promoted to a header: nothing here proves its shape
 * beyond "the thing at data_ov029_02113fd4 that func_ov002_020b676c reads",
 * and three void* is the span that byte-matches, not a recovered layout. The
 * char[] view of that datum is the one the other caller wants, so the cast
 * sits at the use site rather than the declaration. */
struct Arg { void *m[3]; };

extern "C" {
extern int _ZTV15daObjWc_Obj07_c[];
extern int _ZTV16daObjKaitendai_c[];
extern char data_ov029_02113fd4[];
extern int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjWc_Obj07_c_Spawn, 0x02112168, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Obj07_c_Spawn
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
extern "C" int *daObjWc_Obj07_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)&_ZTV15daObjWc_Obj07_c[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN15daObjWc_Obj07_c13InitResourcesEv, 0x02112148, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj07_c13InitResourcesEv
/* The marker above is not decoration: without it tools/tiers.py folds this
 * body into the neighbouring fragment and scores that fragment's mangled
 * vtable externs against InitResources. */
s32 daObjWc_Obj07_c::InitResources()
{
    return func_ov002_020b676c((unsigned char *)this, (struct Arg *)&data_ov029_02113fd4,
                               data_ov029_02113fc4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN15daObjWc_Obj07_c16CleanupResourcesEv, 0x02112134, size 0x14 */
/* -------------------------------------------------------------------------- */
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
//
// The pragma is bracketed on/off around this one function ON PURPOSE. It was
// FUNCTION-scoped when this body lived in its own one-function file; left open
// in a merged TU it becomes TU-scoped and silently relinks the other four
// functions' calls as long calls too. tubuild flagged exactly this at the top
// of the file and refused to carry it automatically.
#pragma long_calls on
// @symbol _ZN15daObjWc_Obj07_c16CleanupResourcesEv
s32 daObjWc_Obj07_c::CleanupResources()
{
    return func_ov002_020b66a8(this, data_ov029_02113fd4);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN15daObjWc_Obj07_cD0Ev, 0x021120d0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj07_cD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV15daObjWc_Obj07_c, VT1 = _ZTV16daObjKaitendai_c */
/* (no separate definition: the single ~daObjWc_Obj07_c() below emits the D0 and D1
 * variants together -- a hand-mangled D0 next to a real destructor is the
 * known mwccarm ICE, ELFgen.c:483.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN15daObjWc_Obj07_cD1Ev, 0x02112080, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj07_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct daObjWc_Obj07_c :
 * daObjKaitendai_c : dBgActor_c` emits its own vptr, then daObjKaitendai_c's --
 * inlined, because that destructor is defined in its class body -- then dBgActor_c's,
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 *
 * There is deliberately no definition here: `~daObjWc_Obj07_c() {}` is INLINE in
 * include/daObjWc_Obj07_c.h, which is what emits D1 and D0 in the ROM's order
 * and suppresses the homeless D2. See the header for why that is required
 * rather than preferred.
 */
