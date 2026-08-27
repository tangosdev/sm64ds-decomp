#ifndef SURFACEINFO_H
#define SURFACEINFO_H

#include "types.h"
#include "CLPS.h"

/* What a collision query is told about the surface it hit: the triangle's CLPS
 * entry, and the face normal.
 *
 * 0x14 BYTES, and the two halves are pinned by different functions.
 *
 * The writer is func_02037eb0, which takes the CLPS entry BY VALUE as a pair of
 * words and the normal by pointer:
 *
 *     o->a = ab.a;  o->b = ab.b;                 -> 0x00, 0x04   the CLPS entry
 *     o->v.x = v->x; o->v.y = v->y; o->v.z = v->z;  -> 0x08..0x13  the normal
 *
 * The reader is SurfaceInfo::CopyNormalTo (0x02037dcc), which copies the
 * Vector3 at 0x08. dBgW_Kc::GetSurfaceInfo (ITCM 0x01ffd920) is what fills
 * one: a real virtual call to GetNormal, then the CLPS lookup, then this.
 *
 * WHY THIS HEADER EXISTS. Before it, seven src/ files each declared their own
 * `struct SurfaceInfo' and no two agreed -- {s32 w0..w4}, {s32 pad}, {char
 * dummy}, two method-only shells, the correct {clps, normal}, and one at 0x24.
 * Seven definitions of one type is how a wrong one silently wins.
 *
 * The 0x24 one, in src/unnamed/ov018/0211/func_ov018_02111bf0.cpp, was not a competing layout at
 * all: it was used as `struct dBgPi { void *vt; SurfaceInfo info; }', i.e. as a
 * filler for the REST of dBgPi (0x28 = 4 + 0x24). It has been renamed there to
 * say what it is, so the name SurfaceInfo now means one thing tree-wide.
 */

/* types.h already defines Vector3 for both languages, so there is one struct
   here and only the method declaration is C++-only. */
struct SurfaceInfo {
    struct CLPS clps;           /* 0x00 - the triangle's collision properties */
    struct Vector3 normal;      /* 0x08 - the face normal */

#ifdef __cplusplus
    void CopyNormalTo(struct Vector3 &out) const;
#endif
};

#ifdef __cplusplus
typedef char SurfaceInfo_size_must_be_0x14[sizeof(SurfaceInfo) == 0x14 ? 1 : -1];
#else
typedef char SurfaceInfo_size_must_be_0x14[sizeof(struct SurfaceInfo) == 0x14 ? 1 : -1];
#endif

#endif /* SURFACEINFO_H */
