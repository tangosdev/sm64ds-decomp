//cpp
// @symbol _Z23LoadUnusedType13ObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadUnusedType13Objects(LVL_Overlay::ObjSubTable&, int, u32) at
 * ov002:0x020fe3e4 -- stash the entry array's ADDRESS and do nothing with it.
 *
 * Not a mistake in the recovery: the ROM really does store the pointer rather
 * than walk it. Object type 13 is unused in the shipped game, so the loader was
 * evidently reduced to a stub that keeps the table around. It reads no count,
 * which fits -- there is nothing to iterate.
 *
 * The file this replaces reached the field as `p[1]' on an `int*', which is the
 * same word at offset 4 by arithmetic rather than by name. *
 * LVL_Overlay IS A CLASS, NOT A NAMESPACE, and the two are indistinguishable in
 * a mangled name -- `N11LVL_Overlay...E' is emitted for either. What settles it
 * is _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider, which takes
 * `LVL_Overlay&': a reference to a namespace does not exist. One of these files
 * used to declare `namespace LVL_Overlay { struct ObjSubTable ... }' and matched
 * perfectly, because nothing about the choice reaches the bytes. */
#include "LVL_Overlay.h"

extern "C" int data_0209f338[];

void LoadUnusedType13Objects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    data_0209f338[0] = (int)tbl.entries;
}
