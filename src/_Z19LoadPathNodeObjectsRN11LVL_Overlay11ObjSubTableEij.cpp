//cpp
// @symbol _Z19LoadPathNodeObjectsRN11LVL_Overlay11ObjSubTableEij
/* LoadPathNodeObjects(LVL_Overlay::ObjSubTable&, int, u32) at ov002:0x020fe6b8
 * -- hand the entry array to func_0203accc along with both trailing arguments.
 *
 * Four instructions: `ldr r0,[r0,#4]' then a tail call. The odd one out among
 * the forwarders in this family -- the other six pass `(entries, count)' to a
 * loader that needs a length, while this one passes `(entries, areaID, param)'
 * and no count at all, so the callee must find its own end. *
 * LVL_Overlay IS A CLASS, NOT A NAMESPACE, and the two are indistinguishable in
 * a mangled name -- `N11LVL_Overlay...E' is emitted for either. What settles it
 * is _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider, which takes
 * `LVL_Overlay&': a reference to a namespace does not exist. One of these files
 * used to declare `namespace LVL_Overlay { struct ObjSubTable ... }' and matched
 * perfectly, because nothing about the choice reaches the bytes. */
#include "LVL_Overlay.h"

extern "C" void func_0203accc(void* entries, int areaID, u32 param);

void LoadPathNodeObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0203accc(tbl.entries, areaID, param);
}
