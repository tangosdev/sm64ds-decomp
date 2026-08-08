//cpp
// @symbol _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadStarCameraObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe3cc
 * -- take the first word of the star-camera table and publish it.
 *
 * Reads no count, unlike most of this family: it wants one value, not a list, so
 * the entry array is dereferenced directly. The other two that skip the count
 * are LoadUnusedType13Objects and LoadPathNodeObjects. *
 * LVL_Overlay IS A CLASS, NOT A NAMESPACE, and the two are indistinguishable in
 * a mangled name -- `N11LVL_Overlay...E' is emitted for either. What settles it
 * is _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider, which takes
 * `LVL_Overlay&': a reference to a namespace does not exist. One of these files
 * used to declare `namespace LVL_Overlay { struct ObjSubTable ... }' and matched
 * perfectly, because nothing about the choice reaches the bytes. */
#include "LVL_Overlay.h"

extern "C" int data_02092134;

void LoadStarCameraObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    data_02092134 = *(int*)tbl.entries;
}
