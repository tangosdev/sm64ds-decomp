//cpp
// @symbol _Z15LoadViewObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadViewObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe690 -- hand the
 * category's entry array and count straight to func_0202b0c4.
 *
 * A free function, not a member: no `this`, and the mangled name has no class
 * prefix. The table type is nested, which is why include/LVL_Overlay.h declares
 * ObjSubTable inside LVL_Overlay rather than beside it -- that nesting is what
 * makes the compiler emit `RN11LVL_Overlay11ObjSubTableE`.
 *
 * The two trailing parameters are declared and unused, here and in every
 * sibling forwarder. They are part of the shared loader signature: LoadObjects
 * calls all of these through one function-pointer table, so they take the same
 * arguments whether they want them or not.
 *
 * This file used to carry its own `struct ObjSubTable`. Six of them existed,
 * character-for-character identical and none of them shared. */
#include "LVL_Overlay.h"

extern "C" void func_0202b0c4(void* entries, u32 count);

void LoadViewObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b0c4(tbl.entries, tbl.count);
}
