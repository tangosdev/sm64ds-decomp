//cpp
// @symbol _ZN12MeshCollider14GetSurfaceInfoEsR11SurfaceInfo
/* MeshCollider::GetSurfaceInfo(s16, SurfaceInfo&) at 0x01ffd920 (ITCM)
 *
 * vtable slot 3. Fills a SurfaceInfo -- the 8-byte CLPS entry describing what the
 * surface is made of, plus its face normal. SurfaceInfo's layout is pinned by
 * SurfaceInfo::CopyNormalTo (0x02037dcc): CLPS entry at 0x0, Vector3 at 0x8.
 *
 * The triangle's `attribute` is passed to the CLPS lookup as a raw index, with no
 * masking -- so in this game the KCL attribute word IS the CLPS index.
 *
 * MATCHING NOTE, worth keeping. GetNormal is called through the vtable, and it has
 * to be a REAL virtual call to match: mwccarm's own dispatch sequence reads `this`
 * from the incoming argument register (`ldr r3,[r0]`), while a hand-rolled
 * `(*(fn**)this)[4](...)` reads it from the callee-saved copy (`ldr r3,[r6]`).
 * That one word was the entire residual through five source variants and a full
 * version sweep; declaration order, hoisting the triangle pointer and caching the
 * attribute are all inert against it.
 */
#include "MeshCollider.h"

/* extern "C" because both callees exist unmangled in config/arm9/symbols.txt --
   giving them C++ linkage would mangle the reference and break the link. */
extern "C" {

/* 8 bytes, passed by value into the SurfaceInfo write below. */
struct CLPS_Entry { u32 a, b; };

/* CLPS lookup at 0x020381cc: hands back the block's entry for `idx`, or a shared
   default when the block is absent or the wrong version. */
void func_020381cc(void *block, int idx, CLPS_Entry **out);

/* SurfaceInfo write at 0x02037eb0: entry into 0x0, normal into 0x8. */
void func_02037eb0(SurfaceInfo *info, CLPS_Entry entry, Vector3 *normal);

}

void MeshCollider::GetSurfaceInfo(s16 triID, SurfaceInfo &res)
{
    CLPS_Entry *entry;
    Vector3 normal;
    KCL_Tri *tri = &kclFile->tris[triID];

    GetNormal(triID, normal);
    func_020381cc(&clps, tri->attribute, &entry);
    func_02037eb0(&res, *entry, &normal);
}
