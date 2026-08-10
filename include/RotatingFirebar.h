#ifndef ROTATINGFIREBAR_H
#define ROTATINGFIREBAR_H

#include "types.h"

/* The rotating bar of flames. Derives from Platform, and the ONE thing it adds
 * is the array of collision cylinders -- one per flame.
 *
 *     CylinderClsnWithPos[8]   0x360 + 8 * 0x3c = 0x540
 *
 * The ROM destroys it with
 * __destroy_arr(this + 0x360, 8, 0x3c, _ZN19CylinderClsnWithPosD1Ev): 0x3c is
 * sizeof(CylinderClsnWithPos) and the element destructor is a name the tree
 * already had, so this class needed no new type.
 */

#ifdef __cplusplus

#include "Platform.h"
#include "CylinderClsnWithPos.h"

struct RotatingFirebar : Platform {
    u8  pad_31e[0x42];
    CylinderClsnWithPos mCylinderClsns[8];   /* 0x360 */

    virtual ~RotatingFirebar();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char RotatingFirebar_size_must_be_0x540[sizeof(RotatingFirebar) == 0x540 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct RotatingFirebar {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x20];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    u8  mModel[0x50];            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
};

#endif /* __cplusplus */

#endif /* ROTATINGFIREBAR_H */
